/**
 @file AHC_Broswer.c
 @brief AHC browser control Function
 @author 
 @version 1.0
*/

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "customer_config.h"
#include "AHC_General.h"
#include "AHC_Browser.h"
#include "AHC_GUI.h"
#include "AHC_UF.h"
#include "AHC_DCF.h"
#include "AIHC_DCF.h"
#include "AIHC_Browser.h"
#include "AIHC_General.h"
#include "AIHC_GUI.h"
#include "mmps_dsc.h"
#include "mmps_vidplay.h"
#include "mmps_audio.h"
#include "mmps_system.h"
#include "AHC_Parameter.h"
#include "AIHC_Display.h"
#include "IconDefine.h"
#include "mmpd_dma.h"
#include "mmpd_graphics.h"
#include "MenuSetting.h"
#include "ColorDefine.h"
#include "AHC_WarningMSG.h"
#include "lcd_common.h"

/** @addtogroup AHC_BROWSER
@{
*/

/*===========================================================================
 * Global variable
 *===========================================================================*/ 

static UINT16 m_CurrentVolume;
static UINT32 m_ulMovPlayFFSpeed        = 5;
static UINT32 m_ulMovPlayRewSpeed       = 5;
extern AHC_BOOL m_ubPlaybackRearCam;
#if (THUMBNAIL_BUFFER_NUMBER == 1)
static UINT32 m_ulDisplayThumbOSD       = THUMBNAIL_OSD_FRONT_ID;
#else
static UINT32 m_ulDisplayThumbOSD       = THUMBNAIL_OSD_BACK_ID;
#endif
UINT32 m_ulDrawingThumbOSD       = THUMBNAIL_OSD_FRONT_ID;

#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
//CCIR656 only
UINT32 m_ulCCIR656buf = 0;
UINT16 m_ulWidth = 0, m_ulHeight = 0;
static volatile AHC_BOOL m_bThumbnailDMADone = AHC_FALSE;
#endif



static UINT32 m_uiFocusIdx          	= 0;

static AHC_THUMB_ATTRIBUTE m_AHCThumb   = {3, 3, 12, 32, 0, 0, 0, 0x101010, 0x0000FF, 0, 0, 240, 180};

/*
0~n-1, n pages
*/
static INT32  m_uiCurPage   			= -1;

static INT32  m_uiMaxIdx    			= -1;

static UINT32 m_ulThumbTempBaseAddr 	= AHC_THUMB_TEMP_BASE_ADDR;

static GUI_BITMAP* m_ProtectKeyTypeMenu = NULL;
static GUI_BITMAP* m_ProtectKeyTypeG    = NULL;
static GUI_BITMAP* m_ProtectKeyTypeMove = NULL;
static GUI_BITMAP* m_FileDamage         = NULL;
static GUI_BITMAP* gpBmpEmrIcon    		= NULL;
#if (EN_SPEED_UP_VID & BROWSER_CASE)
extern UINT16                       m_ulCurrentPBWidth;
extern UINT16                       m_ulCurrentPBHeight;
#endif
extern AHC_BOOL                     gbAhcDbgBrk;
/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
void __TEMP_API__(){}
#endif

#if CCIR656_OUTPUT_ENABLE
AHC_BOOL AHC_Thumb_DrawRectToCCIR656OutputBufferRect(UINT32 ulBufAddr, UINT16 ulWidth, UINT16 ulHeight, UINT16 ulColor,MMP_GRAPHICS_RECT          	*pRect);
#endif

void AIHC_SetTempBrowserMemStartAddr(UINT32 addr) 
{
    m_ulThumbTempBaseAddr = addr;
}

/**
    @brief  Allocate memory pool for Thumb APIs
    @param[in] ptr memory pointer
    @return MMP_ERR_NONE
*/
static void* AIHC_ThumbMalloc(UINT32 size) 
{
    #if 1
    
    return (void*)(m_ulThumbTempBaseAddr);
    
    #else
    UINT8   *ptr;
    UINT32  mem_offset, addr, mem_align = 32;
        
    mem_offset = m_ulThumbTempBaseAddr & (mem_align - 1) ;
    if ( mem_offset != 0)
        mem_offset = mem_align - mem_offset;
    
    addr = (m_ulThumbTempBaseAddr) + size + mem_offset;
    
    ptr = (UINT8*)( m_ulThumbTempBaseAddr + mem_offset);
                    
    m_ulThumbTempBaseAddr += (size + mem_offset);
      
    return (void*)(ptr);
    #endif
}

/**
    @brief  Release memory pool for Thumb APIs
    @param[in] ptr memory pointer
    @return MMP_ERR_NONE
*/
static void AIHC_ThumbFree(void *ptr) 
{
    return;
}

#if 0
void __INTERNAL_API__(){}
#endif

/**
 @brief Get the current thumbnail index

 Retrieve the index of current displayed thumbnail.
 Parameters:
 @param[out] *pwIndex The position of current displayed thumbnail index.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AIHC_Thumb_GetCurrentIdx(UINT32 *pwIndex)
{        
    *pwIndex = m_AHCThumb.uwCurIdx;
    
    return AHC_TRUE;
}

/**
 @brief Set the current thumbnail index

 Set the index of current displayed thumbnail.
 Parameters:
 @param[in] pwIndex The position of current displayed thumbnail index.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AIHC_Thumb_SetCurrentIdx(UINT32 pwIndex)
{        
    m_AHCThumb.uwCurIdx = pwIndex;
    
    return AHC_TRUE;
}

/**
 @brief Keep the index of the DCF object

The mapping between the position and DCF index.
 
 Parameters:
 @param[in] uwPos   The position of the select thumbnail.
 @param[in] uiIdx   The index of the select thumbnail.
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AIHC_Thumb_SetIdxByPos(UINT16 uwPos, UINT32 uiIdx)
{
    if ( uwPos >= MAX_DCF_IN_THUMB )
        return AHC_FALSE;
        
    m_AHCThumb.uiIdxMap[uwPos] = uiIdx;
    
    return AHC_TRUE;
}

AHC_BOOL AIHC_Thumb_DrawFocus(UINT16 uwFocusIndex, AHC_BOOL bClear)
{
#ifndef CFG_CUSTOM_THUMB_DRAW_FOCUS_STYLE // Maybe define in Config_SDK_xxx.h  jeff.li
    UINT16          OSDWidth,OSDHeight,OSDColor,TargetThumbWidth,TargetThumbHeight;
    UINT32          OSDAddr;
    UINT32          ThumbPosX,ThumbPosY;
    #if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL) )
    UINT32          BorderWidth = 4;
    #else
    UINT32          BorderWidth = 1;
    #endif

    #if ( (MENU_STYLE == 1)&& (!STYLE001_SUPPORT_TOUCH_PANEL) )
    UINT32			LastFocusPos;
    #endif
    UINT32			ThumbBound;
    UINT32          FocusRectWidth;
    UINT32          FocusRectHeight;
    UINT32          FocusRectStartX;
    UINT32          FocusRectStartY;
    UINT32          ThumbPerPage;
    UINT32          uiCurPos;
    UINT8           ubClearFlag = AHC_FALSE;
    #if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL) )
    #else
    UINT32 temp;
    #endif
    GUI_COLOR       CurrentGUIColor;
    UINT32 uiMaxIdx;
    UINT32 uiCurPage;
    UINT32 uiFileCount;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }

    if ( ( (BorderWidth*2) >= m_AHCThumb.usThumbHorizontalGap) || ( (BorderWidth*2) >= m_AHCThumb.usThumbVerticalGap) ) {
    
        if ( m_AHCThumb.usThumbHorizontalGap > m_AHCThumb.usThumbVerticalGap ) {
            BorderWidth = m_AHCThumb.usThumbVerticalGap / 2;
        }
        else {
            BorderWidth = m_AHCThumb.usThumbHorizontalGap / 2;
        }
    }
    
    BEGIN_LAYER(m_ulDrawingThumbOSD);
    AIHC_DrawReservedOSD(AHC_TRUE);
    
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetPenSize(m_ulDrawingThumbOSD, BorderWidth);
    CurrentGUIColor = AHC_OSDGetColor(m_ulDrawingThumbOSD);
#else
    AHC_OSDSetPenSize(BorderWidth);
    CurrentGUIColor = AHC_OSDGetColor();
#endif
        
    AHC_OSDGetBufferAttr(m_ulDrawingThumbOSD,&OSDWidth,&OSDHeight,&OSDColor,&OSDAddr);
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    {
        UINT16  ulOSDTemp = 0;

        //Swap width/height. Rotate OSD by GUI.
        ulOSDTemp = OSDWidth;
        OSDWidth = OSDHeight;
        OSDHeight = ulOSDTemp;
    }
#endif
    
    #if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
    TargetThumbWidth = m_AHCThumb.uwTargetHorizontal ? m_AHCThumb.uwTargetHorizontal : (( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) / m_AHCThumb.usThumbPerRow);
    TargetThumbHeight = m_AHCThumb.uwTargetVertical ? m_AHCThumb.uwTargetVertical : (( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) / m_AHCThumb.usThumbPerCol);	
    #else	
    TargetThumbWidth  = ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) / m_AHCThumb.usThumbPerRow;
    TargetThumbHeight = ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) / m_AHCThumb.usThumbPerCol;
    #endif    

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetPenSize(m_ulDrawingThumbOSD, BorderWidth);
#else
    AHC_OSDSetPenSize(BorderWidth);
#endif
    FocusRectWidth  = TargetThumbWidth  + BorderWidth + BorderWidth/2;
    FocusRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;

    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    uiFileCount = uiMaxIdx;

    uiMaxIdx -= 1;
    ThumbPerPage    = m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol;
    uiCurPage  = uwFocusIndex / ThumbPerPage;  
    uiCurPos        = uwFocusIndex - (uiCurPage*ThumbPerPage);

    #if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL) )
    LastFocusPos = uiCurPage*ThumbPerPage + uiCurPos;
    #endif
    
    ThumbPosX       = 1 + uiCurPos - ( uiCurPos / m_AHCThumb.usThumbPerRow ) * m_AHCThumb.usThumbPerRow;
    FocusRectStartX = m_AHCThumb.StartX + (ThumbPosX - 1) * (m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
    
    ThumbPosY       = 1 + uiCurPos / m_AHCThumb.usThumbPerRow;
    FocusRectStartY = m_AHCThumb.StartY + (ThumbPosY - 1) * (m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth;

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulFocusColor);
    #else
    AHC_OSDSetColor(m_AHCThumb.ulFocusColor);
    #endif

    #if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL))
    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                             FocusRectStartX, 
                             FocusRectStartY, 
                             FocusRectStartX + FocusRectWidth, 
                             FocusRectStartY+BorderWidth,2 );
    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                             FocusRectStartX-1, 
                             FocusRectStartY, 
                             FocusRectStartX+BorderWidth-1, 
                             FocusRectStartY + FocusRectHeight+1, 2);
    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                             FocusRectStartX + FocusRectWidth + (BorderWidth/2)-BorderWidth, 
                             FocusRectStartY, 
                             FocusRectStartX + FocusRectWidth + (BorderWidth/2), 
                             FocusRectStartY + FocusRectHeight + (BorderWidth/2), 2); 
    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                             FocusRectStartX, 
                             FocusRectStartY + FocusRectHeight + (BorderWidth/2)-BorderWidth, 
                             FocusRectStartX + FocusRectWidth, 
                             FocusRectStartY + FocusRectHeight + (BorderWidth/2), 2);
    #else
		#if CCIR656_OUTPUT_ENABLE
		{
			MMP_GRAPHICS_RECT          	Rect;
			BorderWidth *=2;
			Rect.usLeft  = FocusRectStartX;
			Rect.usTop=FocusRectStartY-BorderWidth;
			Rect.usWidth = FocusRectWidth;
			Rect.usHeight = BorderWidth + 1;
    		AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			Rect.usLeft = FocusRectStartX-BorderWidth;
			Rect.usTop  =FocusRectStartY-BorderWidth;
			Rect.usWidth = BorderWidth;
			Rect.usHeight = FocusRectHeight+2*BorderWidth;
    		AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			Rect.usLeft =FocusRectStartX + FocusRectWidth;
			Rect.usTop = FocusRectStartY -BorderWidth;
			Rect.usWidth =   BorderWidth;
			Rect.usHeight = FocusRectHeight + 2*(BorderWidth);
    		AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			Rect.usLeft =FocusRectStartX;
			Rect.usTop = FocusRectStartY + FocusRectHeight;
			Rect.usWidth = FocusRectWidth;
			Rect.usHeight =   BorderWidth;
    		AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			{
				INT8                        FilePathName[MAX_FILE_NAME_SIZE];
				INT8                        FileTime[MAX_FILE_NAME_SIZE];
				UINT8 						ubFileType;
				AHC_RTC_TIME RtcTime;

				MEMSET(FilePathName, 0, sizeof(FilePathName));
				AHC_UF_GetFileTypebyIndex(uwFocusIndex, &ubFileType);

				if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){ 
					AHC_UF_SetRearCamPathFlag(AHC_TRUE);
				}

				AHC_UF_GetFilePathNamebyIndex(uwFocusIndex, FilePathName);
				AHC_UF_GetFileTimebyIndex(uwFocusIndex,&RtcTime);

				if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){
					AHC_UF_SetRearCamPathFlag(AHC_FALSE);
				}
				
				sprintf(FileTime,"%d-%d-%d %d:%d:%d\0",RtcTime.uwYear,RtcTime.uwMonth,RtcTime.uwDay,RtcTime.uwHour,RtcTime.uwMinute,RtcTime.uwSecond);
				Rect.usLeft =80;
				Rect.usTop = 430;
				Rect.usWidth = 640;
				Rect.usHeight =   30;
				AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x080/*black*/,&Rect);
				PrintStringYuv422(m_ulCCIR656buf, 720, 720*430+80, 0x0000, 0xff80, 1, FilePathName);
				PrintStringYuv422(m_ulCCIR656buf, 720, 720*430+400, 0x0000, 0x8080, 1, FileTime);
			}
		}
		#else
			AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
								 FocusRectStartX, 
								 FocusRectStartY-BorderWidth, 
								 FocusRectStartX + FocusRectWidth, 
								 FocusRectStartY);
			AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
								 FocusRectStartX-BorderWidth-1, 
								 FocusRectStartY, 
								 FocusRectStartX-1, 
								 FocusRectStartY + FocusRectHeight+1);
			AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
								 FocusRectStartX + FocusRectWidth + (BorderWidth/2), 
								 FocusRectStartY, 
								 FocusRectStartX + FocusRectWidth + (BorderWidth/2) + BorderWidth, 
								 FocusRectStartY + FocusRectHeight + (BorderWidth/2)); 
			AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
								 FocusRectStartX, 
								 FocusRectStartY + FocusRectHeight + (BorderWidth/2), 
								 FocusRectStartX + FocusRectWidth, 
								 FocusRectStartY + FocusRectHeight + (BorderWidth/2) + BorderWidth);
		#endif
    #endif

    #if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL) )
    // NOP
    #else
    BEGIN_LAYER(m_ulDisplayThumbOSD);
    temp                = m_ulDisplayThumbOSD;
    m_ulDisplayThumbOSD = m_ulDrawingThumbOSD;
    m_ulDrawingThumbOSD = temp;

    //AHC_OSDSetActive(m_ulDrawingThumbOSD,AHC_TRUE);
    AHC_OSDSetCurrentDisplay(m_ulDisplayThumbOSD);
    AHC_OSDSetActive(m_ulDisplayThumbOSD,AHC_TRUE);
        
    AHC_OSDRefresh();
    END_LAYER(m_ulDisplayThumbOSD);
    #endif
	
    if (bClear) {
        //Clear the current focus rectangle
        
        ThumbPerPage    = m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol;
        ThumbBound    = uiFileCount - (uiFileCount/ThumbPerPage) * ThumbPerPage;
        
        uiCurPage       = (uiMaxIdx - m_uiFocusIdx) / (ThumbPerPage);
        

        //uiMaxIdxCurPage = uiCurPage*(ThumbPerPage);
        if(uiCurPos != (m_uiFocusIdx % ThumbPerPage))
        {
            uiCurPos = m_uiFocusIdx % ThumbPerPage;
            ubClearFlag = AHC_TRUE;
        }


        //uiMaxIdxCurPage = uiCurPage*(ThumbPerPage);
        if(ubClearFlag)
        {
            
            ThumbPosX       = 1 + uiCurPos - ( uiCurPos / m_AHCThumb.usThumbPerRow ) * m_AHCThumb.usThumbPerRow;
            FocusRectStartX = m_AHCThumb.StartX + (ThumbPosX - 1) * (m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
            
            ThumbPosY       = 1 + uiCurPos / m_AHCThumb.usThumbPerRow;
            FocusRectStartY = m_AHCThumb.StartY + (ThumbPosY - 1) * (m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth;

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
#else
            #ifdef CAR_DV
            AHC_OSDSetColor(m_AHCThumb.ulBkColor);
            #else
            AHC_OSDSetColor(RGB565_COLOR_GRAY); //20150313 Mark to keep the border in thumbnail mode. 
            #endif
#endif
            #if ( (MENU_STYLE == 1)&& (!STYLE001_SUPPORT_TOUCH_PANEL) )
            if ( (uiCurPos + 1) <= uiFileCount) 
            {
				//if ( (uiCurPage*ThumbPerPage+uiCurPos) < uiFileCount ) {
				if ((((uiFileCount/ThumbPerPage)+ 1) > ((LastFocusPos/ThumbPerPage)+ 1)) || 
				   ( ((ThumbPosY-1)*m_AHCThumb.usThumbPerRow+ThumbPosX) <= (ThumbBound)  ) )
				{
					AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
									 FocusRectStartX, 
									 FocusRectStartY, 
									 FocusRectStartX + FocusRectWidth, 
									 FocusRectStartY+BorderWidth, 2);
					AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
									 FocusRectStartX-1, 
									 FocusRectStartY, 
									 FocusRectStartX+BorderWidth-1, 
									 FocusRectStartY + FocusRectHeight+1, 2);
					AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
									 FocusRectStartX + FocusRectWidth + (BorderWidth/2)-BorderWidth, 
									 FocusRectStartY, 
									 FocusRectStartX + FocusRectWidth + (BorderWidth/2), 
									 FocusRectStartY + FocusRectHeight + (BorderWidth/2), 2);
					AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
									 FocusRectStartX, 
									 FocusRectStartY + FocusRectHeight + (BorderWidth/2)-BorderWidth, 
									 FocusRectStartX + FocusRectWidth, 
									 FocusRectStartY + FocusRectHeight + (BorderWidth/2), 2);			
				}
            }
            #else
				#if CCIR656_OUTPUT_ENABLE
				{
					MMP_GRAPHICS_RECT          	Rect;
					BorderWidth *=2;
					Rect.usLeft  = FocusRectStartX;
					Rect.usTop=FocusRectStartY-BorderWidth;
					Rect.usWidth = FocusRectWidth;
					Rect.usHeight = BorderWidth + 2;
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x0080/*black*/,&Rect);

				    Rect.usLeft = FocusRectStartX-BorderWidth;
				    Rect.usTop  =FocusRectStartY-BorderWidth;
				    Rect.usWidth = BorderWidth +2;
				    Rect.usHeight = FocusRectHeight+2*BorderWidth;
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x0080/*black*/,&Rect);

				    Rect.usLeft =FocusRectStartX + FocusRectWidth;
				    Rect.usTop = FocusRectStartY -BorderWidth;
				    Rect.usWidth =   BorderWidth + 2;
				    Rect.usHeight = FocusRectHeight + 2*(BorderWidth);
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x0080/*black*/,&Rect);

				    Rect.usLeft =FocusRectStartX;
				    Rect.usTop = FocusRectStartY + FocusRectHeight;
				    Rect.usWidth = FocusRectWidth;
				    Rect.usHeight =   BorderWidth;
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x0080/*black*/,&Rect);
				}

				#else
        
					AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
										 FocusRectStartX, 
										 FocusRectStartY-BorderWidth, 
										 FocusRectStartX + FocusRectWidth, 
										 FocusRectStartY);
					AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
										 FocusRectStartX-BorderWidth-1, 
										 FocusRectStartY, 
										 FocusRectStartX-1, 
										 FocusRectStartY + FocusRectHeight+1);
					AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
										 FocusRectStartX + FocusRectWidth + (BorderWidth/2), 
										 FocusRectStartY, 
										 FocusRectStartX + FocusRectWidth + (BorderWidth/2)+BorderWidth, 
										 FocusRectStartY + FocusRectHeight + (BorderWidth/2));
					AHC_OSDDrawFillRect( m_ulDrawingThumbOSD, 
										 FocusRectStartX, 
										 FocusRectStartY + FocusRectHeight + (BorderWidth/2), 
										 FocusRectStartX + FocusRectWidth, 
										 FocusRectStartY + FocusRectHeight + (BorderWidth/2)+BorderWidth);
				#endif
            #endif
            ubClearFlag = AHC_FALSE;
        }
    }

    #if ( (MENU_STYLE == 1) && (!STYLE001_SUPPORT_TOUCH_PANEL) )
    BEGIN_LAYER(m_ulDisplayThumbOSD);
    AHC_OSDDuplicateBuffer(m_ulDrawingThumbOSD, m_ulDisplayThumbOSD);
    
    AHC_OSDSetCurrentDisplay(m_ulDisplayThumbOSD);
    AHC_OSDSetActive(m_ulDisplayThumbOSD,AHC_TRUE);

    AHC_OSDRefresh();
    END_LAYER(m_ulDisplayThumbOSD);
    #endif
	
    AHC_UF_SetCurrentIndex((UINT32)uwFocusIndex);
	
    //restore color
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
#else
    AHC_OSDSetColor(CurrentGUIColor);
#endif
    
    m_uiFocusIdx = uwFocusIndex;
    
    AIHC_DrawReservedOSD(AHC_FALSE);
    END_LAYER(m_ulDrawingThumbOSD);
#endif // CFG_CUSTOM_THUMB_DRAW_FOCUS_STYLE

    return AHC_TRUE;
}

GUI_BITMAP* AHC_ICON_GetProtectKey(AHC_PROTECT_TYPE Type)
{
    GUI_BITMAP* psTemp;
    
    switch(Type){
    
    case AHC_PROTECT_G:
        psTemp = m_ProtectKeyTypeG;
        break;
    case AHC_PROTECT_MENU:
        psTemp = m_ProtectKeyTypeMenu;
        break;
    case AHC_PROTECT_MOVE:
        psTemp = m_ProtectKeyTypeMove;
        break;
    default:
        psTemp = NULL;
        break;
    }
    
    return psTemp;
}

void AHC_ICON_LoadProtectKey(AHC_PROTECT_TYPE Type, GUI_BITMAP* psKeyIcon)
{
    switch(Type){
    
    case AHC_PROTECT_G:
        m_ProtectKeyTypeG       = psKeyIcon;
        break;
    case AHC_PROTECT_MENU:
        m_ProtectKeyTypeMenu    = psKeyIcon;
        break;
    case AHC_PROTECT_MOVE:
        m_ProtectKeyTypeMove    = psKeyIcon;
        break;
    default:
        break;
    }
}

void AHC_Thumb_SetEmrBMPIcon(GUI_BITMAP * psEmrIcon)
{
    gpBmpEmrIcon = psEmrIcon;
}

void AHC_ICON_LoadFileDamage(GUI_BITMAP* psFileDamageIcon)
{
    m_FileDamage       = psFileDamageIcon;
}

/**
@brief	draw the icon of the attribute of the file protection
 Parameters:
 @param[in] ThumbIndex  The key of the select DCF object.
 @param[in] IconType 0:Normal lock 1:M lock 2:G lock
 
 @retval AHC_TRUE Success.

*/

AHC_BOOL AIHC_DrawProtectKey(UINT16 ThumbIndex, AHC_PROTECT_TYPE IconType)
{
    UINT16          OSDWidth;
    UINT16          OSDHeight;
    UINT16          OSDColor;
    UINT16          TargetThumbWidth;
    UINT16          TargetThumbHeight;   
    UINT32          OSDAddr;
    UINT32          ThumbPosX;
    UINT32          ThumbPosY;	

    UINT32          FocusRectWidth;
    UINT32          FocusRectHeight;
    UINT32          FocusRectStartX;
    UINT32          FocusRectStartY;
    UINT32          ThumbPerPage;
    UINT32          ThumbOffset;
    GUI_COLOR       CurrentGUIColor; 
    UINT32          BorderWidth = 4;
    GUI_BITMAP*     psProtectIcon;
    UINT8           ubProtectIconOffset = 0;
    UINT8			status;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }
    
    switch(IconType) {
    case AHC_PROTECT_MENU:
        psProtectIcon = m_ProtectKeyTypeMenu;
        break;
    case AHC_PROTECT_G:
        psProtectIcon = m_ProtectKeyTypeG;
        break;
    default:
        psProtectIcon = NULL;
        return AHC_TRUE;
        break;
    }
        
    if( ( (BorderWidth*2) >= m_AHCThumb.usThumbHorizontalGap) || 
        ( (BorderWidth*2) >= m_AHCThumb.usThumbVerticalGap) ){
    
        if ( m_AHCThumb.usThumbHorizontalGap > m_AHCThumb.usThumbVerticalGap ){
            BorderWidth = m_AHCThumb.usThumbVerticalGap / 2;
        }else{
            BorderWidth = m_AHCThumb.usThumbHorizontalGap / 2;
        }
    }
    
    BEGIN_LAYER(m_ulDrawingThumbOSD);
    AIHC_DrawReservedOSD(AHC_TRUE);
    
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetPenSize(m_ulDrawingThumbOSD, BorderWidth);
    CurrentGUIColor = AHC_OSDGetColor(m_ulDrawingThumbOSD);
#else
    AHC_OSDSetPenSize(BorderWidth);
    CurrentGUIColor = AHC_OSDGetColor();
#endif
        
    AHC_OSDGetBufferAttr(	m_ulDrawingThumbOSD,
                            &OSDWidth,
                            &OSDHeight,
                            &OSDColor,
                            &OSDAddr );
                            
    #if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
    TargetThumbWidth  = m_AHCThumb.uwTargetHorizontal ? m_AHCThumb.uwTargetHorizontal : (( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) / m_AHCThumb.usThumbPerRow);
    TargetThumbHeight = m_AHCThumb.uwTargetVertical ? m_AHCThumb.uwTargetVertical : (( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) / m_AHCThumb.usThumbPerCol);
    #else
    TargetThumbWidth  = ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) / m_AHCThumb.usThumbPerRow;
    TargetThumbHeight = ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) / m_AHCThumb.usThumbPerCol;
    #endif
    
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetPenSize(m_ulDrawingThumbOSD, BorderWidth);
#else
    AHC_OSDSetPenSize(BorderWidth);
#endif
        
    FocusRectWidth  = TargetThumbWidth  + BorderWidth + BorderWidth/2;
    FocusRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;
    
    //Draw the select focus rectangle    
    ThumbPerPage    = m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol;
    ThumbOffset     = ThumbIndex % ThumbPerPage;
    
    AHC_LCD_GetStatus(&status);
    
    if(status)
    {        
        ThumbPosX       = (m_AHCThumb.usThumbPerRow - ( ThumbOffset - ( ThumbOffset / m_AHCThumb.usThumbPerRow ) * m_AHCThumb.usThumbPerRow));  
        FocusRectStartX = (ThumbPosX - 1) * (m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
        ubProtectIconOffset = m_AHCThumb.StartX;
    }
    else
    {
        ThumbPosX       = 1 + ThumbOffset - ( ThumbOffset / m_AHCThumb.usThumbPerRow ) * m_AHCThumb.usThumbPerRow;
        FocusRectStartX = m_AHCThumb.StartX + (ThumbPosX - 1) * (m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
        ubProtectIconOffset = 0;
    }
    
    ThumbPosY       = 1 + ThumbOffset / m_AHCThumb.usThumbPerRow;
    FocusRectStartY = m_AHCThumb.StartY + (ThumbPosY - 1) * (m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth; 
    
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetPenSize(m_ulDrawingThumbOSD, BorderWidth);
#else
    AHC_OSDSetPenSize(BorderWidth);
#endif

    if (psProtectIcon) {
#if 1 //Draw PIP window  
        AHC_OSDDrawBitmap(THUMBNAIL_OSD_FRONT_ID, psProtectIcon, FocusRectStartX+ubProtectIconOffset+5, FocusRectStartY+5); 
        AHC_OSDDrawBitmap(THUMBNAIL_OSD_BACK_ID,  psProtectIcon, FocusRectStartX+ubProtectIconOffset+5, FocusRectStartY+5); 
#else //Draw Overlay window 
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_ulDrawingThumbOSD, 0xAA000000);
        #else
        AHC_OSDSetColor(0xAA000000);
        #endif
        AHC_OSDDrawBitmap(16/*OVL_BUFFER_INDEX*/,  psProtectIcon, FocusRectStartX+m_AHCThumb.StartX, FocusRectStartY+m_AHCThumb.StartY); 
        AHC_OSDDrawBitmap(17/*OVL_BUFFER_INDEX1*/, psProtectIcon, FocusRectStartX+m_AHCThumb.StartX, FocusRectStartY+m_AHCThumb.StartY);
#endif
    }
    
    END_LAYER(m_ulDrawingThumbOSD);

    return AHC_TRUE;
}

static AHC_BOOL AIHC_Thumb_CalculateRatio(  UINT32 uiImageWidth, UINT32 uiImageHeight, 
                                            UINT32 uiTargetWidth, UINT32 uiTargetHeight,
                                            UINT16* puiThumbWidth, UINT16* puiThumbHeight)
{
    MMP_BOOL  bSquarePixel;
    MMP_ULONG ulPixelWidth;
    UINT64    uiWidth, uiHeight;
#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        *puiThumbWidth = uiTargetWidth;
        *puiThumbHeight = uiTargetHeight;
        return AHC_TRUE;
    }
#endif

    MMPS_Display_CheckPixelRatio(MMP_DISPLAY_PRM_CTL, &bSquarePixel, &ulPixelWidth);

    uiWidth = uiTargetHeight * uiImageWidth / uiImageHeight;

    // If not square pixel, calculate resolution domain value.
    if (bSquarePixel == MMP_FALSE)
    {
        uiWidth = (uiWidth << 16) / ulPixelWidth;
    }

    // Make sure width is less equal to uiTargetWidth
    if (uiWidth > uiTargetWidth)
    {
        uiWidth  = uiTargetWidth;
    }

    // Calculate height
    uiHeight = uiWidth * uiImageHeight / uiImageWidth;

    // If not square pixel, calculate resolution domain value.
    if (bSquarePixel == MMP_FALSE)
    {
        uiHeight = (uiHeight * ulPixelWidth) >> 16;
    }

    *puiThumbWidth   = (UINT16)uiWidth;
    *puiThumbHeight  = (UINT16)uiHeight;

    return AHC_TRUE;
}

__inline UINT16 AIHC_Thumb_CalculatePos(  UINT16 usTargetPos , 
                                       UINT16 usTargetLength, 
                                       UINT16 usThumbLength )
{
    UINT16 usThumbPos;

    usThumbPos = ( usTargetLength - usThumbLength  ) /2 + usTargetPos;
    
    return usThumbPos;
}

AHC_BOOL AIHC_FlipThumbnailImg(MMP_GRAPHICS_BUF_ATTR *srcbufattribute,UINT32 destinationAddress)
{
    MMP_GRAPHICS_BUF_ATTR   dstbufattribute;
    MMP_GRAPHICS_RECT       srcrect;
    
    srcrect.usLeft                  = 0;
    srcrect.usTop                   = 0;
    srcrect.usWidth                 = srcbufattribute->usWidth;
    srcrect.usHeight                = srcbufattribute->usHeight;
    
    dstbufattribute.usWidth         = srcbufattribute->usWidth;
    dstbufattribute.usHeight        = srcbufattribute->usHeight;
    dstbufattribute.ulBaseAddr      = destinationAddress; 
    dstbufattribute.usLineOffset    = srcbufattribute->usLineOffset;//dstbufattribute.usWidth*2;
    dstbufattribute.colordepth      = srcbufattribute->colordepth;    

    MMPD_DMA_RotateImageBuftoBuf(srcbufattribute,&srcrect,&dstbufattribute,0,0,0,NULL,1,0);
    dstbufattribute.ulBaseAddr 		= srcbufattribute->ulBaseAddr;
    srcbufattribute->ulBaseAddr 	= destinationAddress;
    MMPD_DMA_RotateImageBuftoBuf(srcbufattribute,&srcrect,&dstbufattribute,0,0,0,NULL,0,0);

    return AHC_TRUE;
}

AHC_BOOL AHC_DrawThumbNailEmpty(void)
{
    UINT32		MaxDcfObjIdx = 0;
    UINT16      OSDWidth,OSDHeight,OSDColor;
    UINT32      OSDAddr;
    GUI_COLOR   CurrentGUIColor,CurrentBkColor;
   	AHC_DISPLAY_OUTPUTPANEL  	OutputDevice;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }
    
    BEGIN_LAYER(m_ulDrawingThumbOSD);

    AIHC_DrawReservedOSD(AHC_TRUE);

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    CurrentGUIColor = AHC_OSDGetColor(m_ulDrawingThumbOSD);
    CurrentBkColor  = AHC_OSDGetBkColor(m_ulDrawingThumbOSD);
#else
    CurrentGUIColor = AHC_OSDGetColor();
    CurrentBkColor  = AHC_OSDGetBkColor();
#endif
    
    AHC_OSDGetBufferAttr(m_ulDrawingThumbOSD,&OSDWidth,&OSDHeight,&OSDColor,&OSDAddr);
   	AHC_GetDisplayOutputDev(&OutputDevice);
   	if(OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD) {
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
       	{
            UINT16  ulOSDTemp = 0;
           	//Swap width/height. Rotate OSD by GUI.
           	ulOSDTemp = OSDWidth;
           	OSDWidth = OSDHeight;
           	OSDHeight = ulOSDTemp;
       	}
#endif
    }
    if ( MaxDcfObjIdx == 0 ) {
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        #else
        AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_AHCThumb.ulBkColor);
        #endif

        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, 0, 0, OSDWidth, OSDHeight);
        
        AHC_OSDSetCurrentDisplay(m_ulDrawingThumbOSD);
        AHC_OSDSetActive(m_ulDrawingThumbOSD,AHC_TRUE);
        
        AHC_OSDRefresh();
        
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
        #else
        AHC_OSDSetColor(CurrentGUIColor);
        AHC_OSDSetBkColor(CurrentBkColor);
        #endif

        AIHC_DrawReservedOSD(AHC_FALSE);

        m_uiCurPage          = -1;

        m_uiMaxIdx       = -1;

        END_LAYER(m_ulDrawingThumbOSD);

        return AHC_TRUE;
    }

    AIHC_DrawReservedOSD(AHC_FALSE);

    END_LAYER(m_ulDrawingThumbOSD);

    return AHC_TRUE;
}

#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
AHC_BOOL AHC_Thumb_SetCCIR656OutputBuffer(UINT32 ulCCIR656Buf, UINT16 ulWidth, UINT16 ulHeight)
{
    m_ulCCIR656buf = ulCCIR656Buf;
    m_ulWidth = ulWidth;
    m_ulHeight = ulHeight;
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetCCIR656OutputBuffer(UINT32 *pulCCIR656Buf, UINT16 *pulWidth, UINT16 *pulHeight)
{    
    *pulCCIR656Buf = m_ulCCIR656buf;
    *pulWidth = m_ulWidth;
    *pulHeight = m_ulHeight;

    return AHC_TRUE;
}

static void AHC_Thumb_DMADone(void)
{
    m_bThumbnailDMADone = AHC_TRUE;
}

AHC_BOOL AHC_Thumb_DrawRectToCCIR656OutputBuffer(UINT32 ulBufAddr, UINT16 ulWidth, UINT16 ulHeight, UINT16 ulColor)
{
    MMP_GRAPHICS_DRAWRECT_ATTR  drawAttr;
    MMP_ERR                     err;
    
    drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
    drawAttr.bUseRect   = MMP_FALSE;
    drawAttr.usWidth    = ulWidth;    
    drawAttr.usHeight   = ulHeight;
    drawAttr.usLineOfst = ulWidth * 2;
    drawAttr.ulBaseAddr = ulBufAddr;//m_ulCCIR656buf;//ulBufAddr;
    drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
    drawAttr.ulColor    = ulColor; //For YUV422
    drawAttr.ulPenSize  = 0;
    drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;

    err = MMPD_Graphics_DrawRectToBuf(&drawAttr, NULL, NULL);
    if(err != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;}                 

    return AHC_TRUE;    
}

AHC_BOOL AHC_Thumb_DrawRectToCCIR656OutputBufferRect(UINT32 ulBufAddr, UINT16 ulWidth, UINT16 ulHeight, UINT16 ulColor,MMP_GRAPHICS_RECT          	*pRect)
{
    MMP_GRAPHICS_DRAWRECT_ATTR  drawAttr;
    MMP_ERR                     err;
    
    drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
    drawAttr.bUseRect   = MMP_TRUE;
    drawAttr.usWidth    = ulWidth;    
    drawAttr.usHeight   = ulHeight;
    drawAttr.usLineOfst = ulWidth * 2;
    drawAttr.ulBaseAddr = m_ulCCIR656buf;//ulBufAddr; modify by zhengbo,because this function use for ccir656 project only.
    drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
    drawAttr.ulColor    = ulColor; //For YUV422
    drawAttr.ulPenSize  = 0;
    drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;

    err = MMPD_Graphics_DrawRectToBuf(&drawAttr, pRect, NULL);
    if(err != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;}                 

    return AHC_TRUE;    
}
AHC_BOOL AHC_Thumb_CopyFrameToCCIR656OutputBuffer(UINT32 ulBufAddr, UINT16 ulStartX, UINT16 ulStartY, UINT16 ulWidth, UINT16 ulHeight)
{
    UINT16  				ulCcirOutW;
    UINT16  				ulCcirOutH;
    MMP_GRAPHICS_DRAWRECT_ATTR  drawAttr;
    MMP_GRAPHICS_BUF_ATTR       srcBufAttr;
    MMP_GRAPHICS_BUF_ATTR 	    dstBufAttr;
    MMP_GRAPHICS_RECT          	rect;
    MMP_ERR                     err;
    UINT32 ulCCIR656Buf = 0, ulTimeout = 100000;
    
    AHC_Thumb_GetCCIR656OutputBuffer(&ulCCIR656Buf, &ulCcirOutW, &ulCcirOutH);
    if(ulCCIR656Buf == 0){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;}                 

    //ulCcirOutW = MMPS_Display_GetConfig()->ccir.usDisplayWidth;
    //ulCcirOutH = MMPS_Display_GetConfig()->ccir.usDisplayHeight;
#if 0
    /* Fill background color with black */
    if ((ulWidth != ulCcirOutW) ||(ulHeight != ulCcirOutH)) {

        drawAttr.type       = MMP_GRAPHICS_SOLID_FILL;
        drawAttr.bUseRect   = MMP_FALSE;
        drawAttr.usWidth    = ulCcirOutW;    
        drawAttr.usHeight   = ulCcirOutH;
        drawAttr.usLineOfst = ulCcirOutW * 2;
        drawAttr.ulBaseAddr = ulCcirFrameBuf;
        drawAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
        drawAttr.ulColor    = 0x0080; //For YUV422
        drawAttr.ulPenSize  = 0;
        drawAttr.ropcode    = MMP_GRAPHICS_ROP_SRCCOPY;

        MMPD_Graphics_DrawRectToBuf(&drawAttr, NULL, NULL);
    }
#endif

    /* Move decoded frame to ccir display buffer */
    rect.usLeft             = 0;
    rect.usTop              = 0;
    rect.usWidth            = ulWidth;
    rect.usHeight           = ulHeight;

    srcBufAttr.ulBaseAddr 	= ulBufAddr;
    srcBufAttr.usWidth		= ulWidth;
    srcBufAttr.usHeight	    = ulHeight;
    srcBufAttr.usLineOffset = srcBufAttr.usWidth << 1;
    srcBufAttr.colordepth   = MMP_GRAPHICS_COLORDEPTH_16;

    dstBufAttr.ulBaseAddr	= ulCCIR656Buf;
    dstBufAttr.usWidth		= ulCcirOutW;
    dstBufAttr.usHeight	    = ulCcirOutH;
    dstBufAttr.usLineOffset = dstBufAttr.usWidth << 1;
    dstBufAttr.colordepth	= MMP_GRAPHICS_COLORDEPTH_16;

    m_bThumbnailDMADone = AHC_FALSE;

    err = MMPD_DMA_RotateImageBuftoBuf( &srcBufAttr, &rect, &dstBufAttr, 
                                                ulStartX, ulStartY, 
                                                MMP_GRAPHICS_ROTATE_NO_ROTATE, 
                                                (DmaCallBackFunc *)AHC_Thumb_DMADone, 
                                                MMP_FALSE, DMA_NO_MIRROR);    
    if(err != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;}                 

    // Waiting the DMA move buf to buf done.
    while((AHC_FALSE == m_bThumbnailDMADone) && (--ulTimeout > 0));
    if(ulTimeout == 0){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;}                 

#if 0
    /* Refresh ccir display */
    MMPD_CCIR_SetDisplayBuf(ulCcirFrameBuf, 0, 0);
    MMPD_CCIR_SetDisplayRefresh();
#endif

    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetJPGColorFormat(UINT16 OSDColor, MMP_DISPLAY_COLORMODE *pDisplayColorFormat)
{
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        *pDisplayColorFormat = MMP_DISPLAY_COLOR_YUV422;
        return AHC_TRUE;
    }
    else{                     
        if ( OSDColor == OSD_COLOR_RGB565 ) {
            *pDisplayColorFormat = MMP_DISPLAY_COLOR_RGB565;
        }
        else if ( OSDColor == OSD_COLOR_RGB888 ) {
            *pDisplayColorFormat = MMP_DISPLAY_COLOR_RGB888;
        }
        else{
            AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;
        }
    }
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetVideoFrameColorFormat(UINT16 OSDColor, MMP_GRAPHICS_COLORDEPTH *pGraphicColorFormat)
{
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        *pGraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_YUV422_YVYU;
        return AHC_TRUE;
    }
    else{
        if ( OSDColor == OSD_COLOR_RGB565 ) {
            *pGraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_16;
        }
        else if ( OSDColor == OSD_COLOR_RGB888 ) {
            *pGraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_24;
        }
        else{
            AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_FALSE;
        }
    }
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetJPGFrame(INT8 *pFilePathName, UINT32 *pulTargetThumbAddr, UINT16 OSDColor,
                                                            UINT16 ulTargetThumbWidth, UINT16 ulTargetThumbHeight, 
                                                            UINT16 *pusTmpW, UINT16 *pusTmpH)    
{
    MMP_DSC_JPEG_INFO       jpeginfo;
    MMP_GRAPHICS_BUF_ATTR   jpegbufattribute;
    UINT16 uwBufWidth;
    UINT16 uwBufHeight;
    MMP_DISPLAY_COLORMODE   DisplayColor;    
    MMP_ERR sRet = MMP_ERR_NONE;
    AHC_BOOL ahc_ret = AHC_TRUE;
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        //No need to draw thumbnail.
        DisplayColor = MMP_DISPLAY_COLOR_YUV422;
    }
    else{
        ahc_ret = AHC_Thumb_GetJPGColorFormat(OSDColor, &DisplayColor);
        if(ahc_ret != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahc_ret); return AHC_FALSE;} 
    }
    
    *pulTargetThumbAddr = 0;
	MEMSET(&jpeginfo, 0, sizeof(MMP_DSC_JPEG_INFO));
    //MEMSET(jpeginfo.bJpegFileName, 0, DSC_MAX_FILE_NAME_SIZE);

    STRCPY(jpeginfo.bJpegFileName, pFilePathName);

    jpeginfo.usJpegFileNameLength   = STRLEN(pFilePathName);
    jpeginfo.ulJpegBufAddr          = 0;
    jpeginfo.ulJpegBufSize          = 0;
    jpeginfo.bDecodeThumbnail       = MMP_TRUE;
#if (DSC_SUPPORT_BASELINE_MP_FILE)
    jpeginfo.bDecodeLargeThumb 		= MMP_TRUE;
#endif
    jpeginfo.bValid                 = MMP_FALSE;

#if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
    uwBufWidth =  ((*pusTmpW) >> 2) << 2;
    uwBufHeight = ((*pusTmpH)>> 2) << 2;
#else
    uwBufWidth =  ((*pusTmpW+15) >> 4) << 4;
    uwBufHeight = ((*pusTmpH+15)>> 4) << 4;
#endif

    sRet = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
        uwBufWidth,//((usTmpW + 15) >> 4) << 4,
        uwBufHeight,//((usTmpH + 15) >> 4) << 4,
        DisplayColor);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    if( jpeginfo.usThumbHeight == 0 )
    {
        AIHC_Thumb_CalculateRatio( jpeginfo.usPrimaryWidth, jpeginfo.usPrimaryHeight, ulTargetThumbWidth, ulTargetThumbHeight, pusTmpW, pusTmpH);

        sRet = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                uwBufHeight,//((usTmpH + 15) >> 4) << 4,
                DisplayColor);
        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
    }

    sRet = MMPS_DSC_GetJpegDispBufAttr(&jpegbufattribute);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    *pulTargetThumbAddr = jpegbufattribute.ulBaseAddr;

    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetVideoFrame(INT8 *pFilePathName, UINT32 ulTargetThumbAddr, UINT16 OSDColor,
                                                            UINT16 ulTargetThumbWidth, UINT16 ulTargetThumbHeight, 
                                                            UINT16 *pusTmpW, UINT16 *pusTmpH)    
{
    MMPS_VIDEO_CONTAINER_INFO info;
    MMP_ULONG  ulTargetThumbSize;
    MMPS_VIDPLAY_SCALERCONFIG sScalerCfg;
    MMP_GRAPHICS_COLORDEPTH GraphicColorFormat;
    MMP_ERR sRet = MMP_ERR_NONE;
    AHC_BOOL ahc_ret = AHC_TRUE;    
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY;       
    }
    else{
        ahc_ret = AHC_Thumb_GetVideoFrameColorFormat(OSDColor, &GraphicColorFormat);
        if(ahc_ret != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,ahc_ret); return AHC_FALSE;} 
    }
#if !(EN_SPEED_UP_VID & BROWSER_CASE)    
    sRet = MMPS_VIDPLAY_GetContainerInfo(pFilePathName, STRLEN(pFilePathName), &info); 
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

	MMPS_VIDPLAY_SetScalerCfg(&sScalerCfg, MMP_FALSE, 0, 0, 0, 0);
    MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, MMP_DISPLAY_ROTATE_NO_ROTATE, MMP_FALSE, NULL, &sScalerCfg);

    AIHC_Thumb_CalculateRatio( info.video_info[0].width, info.video_info[0].height, 
        ulTargetThumbWidth, ulTargetThumbHeight, 
        pusTmpW, pusTmpH);
#else
	MMPS_VIDPLAY_SetScalerCfg(&sScalerCfg, MMP_FALSE, 0, 0, 0, 0);
    MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, MMP_DISPLAY_ROTATE_NO_ROTATE, MMP_FALSE, NULL, &sScalerCfg);
    MMPS_VIDPLAY_SetThumbSize(ulTargetThumbWidth, ulTargetThumbHeight);
#endif
    sRet = MMPS_VIDPLAY_GetFrame(pFilePathName, STRLEN(pFilePathName), 
        pusTmpW, pusTmpH, 
        GraphicColorFormat, (MMP_USHORT*)ulTargetThumbAddr, 
        &ulTargetThumbSize, MMPS_3GP_GETFRAME_FIRST);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_CopyVideoFrameToOSD(UINT16 uwDisplayID, UINT32 TargetThumbAddr, 
                                                                UINT16 OSDColor, AHC_BOOL bFlip,
                                                                UINT16 TargetThumbWidth, UINT16 TargetThumbHeight,
                                                                UINT16 usTmpW, UINT16 usTmpH,
                                                                UINT32 ThumbPosX,UINT32 ThumbPosY)
{
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        //Here is no OSD buffer.
        AHC_Thumb_CopyFrameToCCIR656OutputBuffer(TargetThumbAddr, 
                                                                AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                                                AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                                                usTmpW, usTmpH);
    }
    else{
        AHC_OSDDrawBuffer(uwDisplayID,
            (UINT8*)TargetThumbAddr,
            AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
            AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
            usTmpW,
            usTmpH,
            OSDColor,bFlip);        
#if 0 //JPG
                        AIHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                                          (UINT8*)jpegbufattribute.ulBaseAddr,
                                          AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                          AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                          uwBufWidth,//usTmpW,
                                          uwBufHeight,//usTmpH,
                                          uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                                          OSDColor);

#endif

    }
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_CopyEmrIconToOSD(UINT16 uwDisplayID, GUI_BITMAP *pEmrIcon, 
                                                                UINT32 ThumbPosX,UINT32 ThumbPosY)
{
    AHC_DISPLAY_OUTPUTPANEL sOutputDevice;
    AHC_GetDisplayOutputDev(&sOutputDevice);

    if(AHC_DISPLAY_CCIR == sOutputDevice){
        //Here is no OSD buffer.
    }
    else{        
        if(pEmrIcon){
            if(AHC_UF_GetDB() == DCF_DB_TYPE_3RD_DB)
            AHC_OSDDrawBitmap(uwDisplayID, pEmrIcon, ThumbPosX, ThumbPosY);
        }
    }
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_DrawDamageVideoFrameToOSD(UINT16 uwDisplayID, GUI_COLOR guiColor, GUI_BITMAP *pFileDamage,
                                                                UINT16 TargetThumbWidth, UINT16 TargetThumbHeight,
                                                                UINT32 ThumbPosX,UINT32 ThumbPosY)
{   
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(uwDisplayID, guiColor);
#else
    AHC_OSDSetColor(guiColor);
#endif

    if(pFileDamage == NULL) {
        AHC_OSDDrawFillRect(uwDisplayID,
        ThumbPosX,
        ThumbPosY,
        ThumbPosX + TargetThumbWidth,
        ThumbPosY + TargetThumbHeight);
    }
    else {
        ThumbPosX += (TargetThumbWidth - pFileDamage->XSize) >> 1;
        ThumbPosY += (TargetThumbHeight - pFileDamage->YSize) >> 1;

        AHC_OSDDrawBitmap(uwDisplayID, pFileDamage, ThumbPosX, ThumbPosY + 1);
    }
    
    return AHC_TRUE;
}

#ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
AHC_BOOL AHC_Thumb_CalculateThumbnailBorder(AHC_THUMB_ATTRIBUTE *pAHCThumb, UINT32 ulCurPos, UINT32 BorderWidth, UINT32 ulOffset,
                                                                UINT16 TargetThumbWidth, UINT16 TargetThumbHeight,
                                                                UINT32 tmpThumbPosX,UINT32 tmpThumbPosY,
                                                                UINT32 *pCurrRectWidth, UINT32 *pCurrRectHeight,
                                                                UINT32 *pCurrRectStartX, UINT32 *pCurrRectStartY)
{
    tmpThumbPosX = ulCurPos-((ulCurPos-ulOffset)/pAHCThumb->usThumbPerRow)*pAHCThumb->usThumbPerRow;
    tmpThumbPosY = 1+(ulCurPos-ulOffset)/pAHCThumb->usThumbPerRow;
    
    *pCurrRectWidth = TargetThumbWidth + BorderWidth + BorderWidth/2;
    *pCurrRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;
    *pCurrRectStartX = pAHCThumb->StartX + (tmpThumbPosX-ulOffset)*(pAHCThumb->usThumbHorizontalGap + TargetThumbWidth) + pAHCThumb->usThumbHorizontalGap - BorderWidth;
    *pCurrRectStartY = pAHCThumb->StartY + (tmpThumbPosY-ulOffset)*(pAHCThumb->usThumbVerticalGap + TargetThumbHeight) + pAHCThumb->usThumbVerticalGap - BorderWidth;
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_DrawThumbnailBorder(UINT16 uwDisplayID, GUI_COLOR guiColor, UINT32 BorderWidth,
                                                                UINT32 CurrRectWidth, UINT32 CurrRectHeight,
                                                                UINT32 CurrRectStartX, UINT32 CurrRectStartY,
                                                                UINT32 tmpThumbPosX,UINT32 tmpThumbPosY)
{
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(uwDisplayID, guiColor);
#else
    AHC_OSDSetColor(guiColor);
#endif

#if CCIR656_OUTPUT_ENABLE
{
    MMP_GRAPHICS_RECT          	Rect;
    BorderWidth *=2;
    Rect.usLeft  = CurrRectStartX;
    Rect.usTop=CurrRectStartY-BorderWidth;
    Rect.usWidth = CurrRectWidth;
    Rect.usHeight = BorderWidth + 1;
    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

    Rect.usLeft = CurrRectStartX-BorderWidth;
    Rect.usTop  =CurrRectStartY-BorderWidth;
    Rect.usWidth = BorderWidth;
    Rect.usHeight = CurrRectHeight+2*BorderWidth;
    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

    Rect.usLeft =CurrRectStartX + CurrRectWidth;
    Rect.usTop = CurrRectStartY -BorderWidth;
    Rect.usWidth =   BorderWidth;
    Rect.usHeight = CurrRectHeight + 2*(BorderWidth);
    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

    Rect.usLeft =CurrRectStartX;
    Rect.usTop = CurrRectStartY + CurrRectHeight;
    Rect.usWidth = CurrRectWidth;
    Rect.usHeight =   BorderWidth;
    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);


}

#else
    AHC_OSDDrawFillRoundedRect( uwDisplayID,
        CurrRectStartX,
        CurrRectStartY,
        CurrRectStartX + CurrRectWidth,
        CurrRectStartY + BorderWidth, 2);
    AHC_OSDDrawFillRoundedRect( uwDisplayID,
        CurrRectStartX-1,
        CurrRectStartY,
        CurrRectStartX+BorderWidth-1,
        CurrRectStartY + CurrRectHeight+1, 2);
    AHC_OSDDrawFillRoundedRect( uwDisplayID,
        CurrRectStartX + CurrRectWidth + (BorderWidth/2)-BorderWidth, 
        CurrRectStartY,
        CurrRectStartX + CurrRectWidth + (BorderWidth/2),
        CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2); 
    AHC_OSDDrawFillRoundedRect( uwDisplayID,
        CurrRectStartX,
        CurrRectStartY + CurrRectHeight + (BorderWidth/2)-BorderWidth, 
        CurrRectStartX + CurrRectWidth,
        CurrRectStartY + CurrRectHeight + (BorderWidth/2),2);
#endif
    return AHC_TRUE;
}
#endif
#endif

AHC_BOOL AHC_Thumb_DrawPage(AHC_BOOL bForce)
{
    UINT32                      uiCurIdx;
    UINT32                      uiCurPage;
    UINT32                      uiMaxIdx;
    UINT32                      uiThumbPerPage;
    UINT32                      uiPosY, uiPosX;
    UINT32						uiDecodeIdx;
    
    UINT8 						ubFileType;
    INT8                        FilePathName[MAX_FILE_NAME_SIZE];
    UINT16                      OSDWidth,OSDHeight,OSDColor,TargetThumbWidth,TargetThumbHeight;
    UINT16                      usTmpW,usTmpH;
    UINT32                      OSDAddr;
    UINT32                      TargetThumbAddr,ThumbPosX,ThumbPosY;	
    UINT16                      ThumbIndex;
    MMP_ULONG                   TargetThumbSize;
    #ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
    UINT32 				        BorderWidth = 4;
    UINT32						CurrRectWidth;
    UINT32						CurrRectHeight;
    UINT32						CurrRectStartX;
    UINT32						CurrRectStartY;
    UINT32						tmpThumbPosX;
    UINT32						tmpThumbPosY;
    #endif
#if (EN_SPEED_UP_VID & BROWSER_CASE)	
    UINT32						t1,t2;
#endif
    GUI_COLOR                   CurrentGUIColor,CurrentBkColor;
    MMP_UBYTE					ubLCDstatus = 0;
    AHC_DISPLAY_OUTPUTPANEL  	OutputDevice;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }

    BEGIN_LAYER(m_ulDrawingThumbOSD);

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AIHC_DrawReservedOSD(AHC_TRUE);
    CurrentGUIColor = AHC_OSDGetColor(m_ulDrawingThumbOSD);
    CurrentBkColor  = AHC_OSDGetBkColor(m_ulDrawingThumbOSD);
    #else
    CurrentGUIColor = AHC_OSDGetColor();
    CurrentBkColor  = AHC_OSDGetBkColor();
    AIHC_DrawReservedOSD(AHC_TRUE);
    #endif

    AHC_OSDGetBufferAttr(m_ulDrawingThumbOSD, &OSDWidth, &OSDHeight, &OSDColor, &OSDAddr);
   	AHC_GetDisplayOutputDev(&OutputDevice);
   	if(OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD) {
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
       	{
           	UINT16  ulOSDTemp = 0;

           	//Swap width/height. Rotate OSD by GUI.
           	ulOSDTemp = OSDWidth;
           	OSDWidth = OSDHeight;
           	OSDHeight = ulOSDTemp;
       }
#endif
    }

    if( AHC_FALSE == AHC_UF_GetTotalFileCount(&uiMaxIdx) )
    {
        if(AHC_UF_IsDBMounted()) {
            AIHC_DrawReservedOSD(AHC_FALSE);
            END_LAYER(m_ulDrawingThumbOSD);

            return AHC_FALSE;
        }
    }
    
    /*
    There is no dcf file in the db.
    
    */
    if ( uiMaxIdx == 0 ) {
        //No DCF obj
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        #else
        AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_AHCThumb.ulBkColor);
        #endif

        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, 0, 0, OSDWidth, OSDHeight);

        AHC_OSDSetCurrentDisplay(m_ulDrawingThumbOSD);
        AHC_OSDSetActive(m_ulDrawingThumbOSD,AHC_TRUE);

        AHC_OSDRefresh();
        
        //restore color
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
        #else
        AHC_OSDSetColor(CurrentGUIColor);
        AHC_OSDSetBkColor(CurrentBkColor);
        #endif
    
        AIHC_DrawReservedOSD(AHC_FALSE);
        
        m_uiCurPage = -1;

        m_uiMaxIdx  = -1;

        END_LAYER(m_ulDrawingThumbOSD);
        
        return AHC_TRUE;
    }

    AHC_GetDisplayOutputDev(&OutputDevice);

    if(OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD) {
    
        #define HORIZONTAL_RATIO    (4)
        #define VERTICAL_RATIO      (3)

        #if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
        { // 4:3
        UINT32 uiX;
        UINT32 uiY;
        
        uiX =   ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) 
                / (HORIZONTAL_RATIO*m_AHCThumb.usThumbPerRow);
        
        uiY =   ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) 
                / (VERTICAL_RATIO*m_AHCThumb.usThumbPerCol);
                            
        if(uiX > uiY){
            TargetThumbWidth    = HORIZONTAL_RATIO*uiY;
            TargetThumbHeight   = VERTICAL_RATIO*uiY;
        }else{
            TargetThumbWidth    = HORIZONTAL_RATIO*uiX;
            TargetThumbHeight   = VERTICAL_RATIO*uiX;
        }

        
        }
        #else
        
        {
        TargetThumbWidth  = ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) 
                            / m_AHCThumb.usThumbPerRow;
                            
        TargetThumbHeight = ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) 
                            / m_AHCThumb.usThumbPerCol;
        }
        #endif
    }
    else {
        TargetThumbWidth  = ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) 
                            / m_AHCThumb.usThumbPerRow;
                            
        TargetThumbHeight = ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) 
                            / m_AHCThumb.usThumbPerCol;	

		
		if(AHC_DISPLAY_CCIR == OutputDevice)
		{
			TargetThumbWidth = (TargetThumbWidth >> 3) << 3; //Align 8
			TargetThumbHeight = (TargetThumbHeight >> 3) << 3; //Align 8
		}
    }	

    m_AHCThumb.uwTargetHorizontal   = TargetThumbWidth;
    m_AHCThumb.uwTargetVertical     = TargetThumbHeight;
    
    AIHC_Thumb_CalculateRatio( 160, 120, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);
    TargetThumbAddr   = (UINT32)AIHC_ThumbMalloc(TargetThumbWidth * TargetThumbHeight * 4);//0x1700000 + TargetThumbWidth * TargetThumbHeight * 4;
    
    // the cuurent dcf index in the db.
    AHC_UF_GetCurrentIndex(&uiCurIdx);
	
    // the boundary check of index.
    if ( uiCurIdx >= (uiMaxIdx - 1) ){
        uiCurIdx = (uiMaxIdx - 1);
    }
    
    // the max index in the db
    uiMaxIdx -= 1;
    
    // The max number per page
    uiThumbPerPage = m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol;
    
    // The current page => 0~n-1, n pages
    uiCurPage = uiCurIdx / uiThumbPerPage;
    
    // The last index in the current page.
    //uiDecodeIdx = (uiCurPage + 1) * uiThumbPerPage - 1;  
    // The first index of the current page. 
    uiDecodeIdx = uiCurPage * uiThumbPerPage;
    /*
        the boundary check of index.
	*/
    if ( uiDecodeIdx > uiMaxIdx ) {

        uiThumbPerPage = (uiMaxIdx + 1) % (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
        
        if(uiThumbPerPage == 0)
            uiThumbPerPage = (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
            
        uiDecodeIdx = uiMaxIdx;
    }
    
	//Redraw the thumbnail only if not the same page or some DCF items are removed
	/*
	The max number of file is not the same as the previous enter.
	*/
    if ( ( uiMaxIdx != m_uiMaxIdx ) || (m_uiCurPage != uiCurPage) || bForce)
    {
        UINT32 k;
        #ifdef WMSG_SHOW_WAIT
        AHC_WMSG_Draw(AHC_TRUE, WMSG_OPENFILE_WAIT, 0);
        #endif
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        #else
        AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_AHCThumb.ulBkColor);
        #endif

		#if CCIR656_OUTPUT_ENABLE
		{
			UINT16	OSDWidth1,OSDHeight1,OSDColor1;
			UINT32	OSDAddr1;
			//clear browser buffer	
			AIHC_DrawReservedOSD(AHC_TRUE);
			AHC_OSDGetBufferAttr(m_ulDisplayThumbOSD, &OSDWidth1, &OSDHeight1, &OSDColor1, &OSDAddr1);
			AHC_Thumb_SetCCIR656OutputBuffer((UINT32)OSDAddr1, OSDWidth1, OSDHeight1);
			AHC_Thumb_DrawRectToCCIR656OutputBuffer((UINT32)OSDAddr1, OSDWidth1, OSDHeight1, 0x0080/*black*/);

			//show thumb buffer
			BEGIN_LAYER(m_ulDisplayThumbOSD);
			AHC_OSDSetCurrentDisplay(m_ulDisplayThumbOSD);
			//AHC_OSDSetActive(m_ulDisplayThumbOSD,AHC_TRUE);
			AHC_OSDRefresh();
			END_LAYER(m_ulDisplayThumbOSD);
			AIHC_DrawReservedOSD(AHC_FALSE);
		}
		
		AHC_Thumb_SetCCIR656OutputBuffer((UINT32)OSDAddr, OSDWidth, OSDHeight);
	 	AHC_Thumb_DrawRectToCCIR656OutputBuffer((UINT32)OSDAddr, OSDWidth, OSDHeight, 0x0080/*black*/);

		#else
        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, 0, 0, OSDWidth, OSDHeight);
		#endif

        k = 1;

        do {
            
            MMP_ERR error;
            
            if ( (k % m_AHCThumb.usThumbPerRow) == 0 ) {
                uiPosY = k / m_AHCThumb.usThumbPerRow ;
                uiPosX = m_AHCThumb.usThumbPerRow;
            }
            else {
                uiPosY = k / m_AHCThumb.usThumbPerRow + 1;
                uiPosX = k % m_AHCThumb.usThumbPerRow;
            }
			#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
			#else            
            AIHC_Thumb_CalculateRatio( 160, 120, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);
			#endif            
            /*
			Get the file information.
			*/
            MEMSET(FilePathName, 0, sizeof(FilePathName));
            AHC_UF_GetFileTypebyIndex(uiDecodeIdx, &ubFileType);
            if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){ 
                AHC_UF_SetRearCamPathFlag(AHC_TRUE);
            }
            AHC_UF_GetFilePathNamebyIndex(uiDecodeIdx, FilePathName);
            printc("Idx : %d, name : %s \r\n", uiDecodeIdx, FilePathName);
            if( m_ubPlaybackRearCam == AHC_TRUE && VIDRECD_MULTI_TRACK == 0 ){
                AHC_UF_SetRearCamPathFlag(AHC_FALSE);
            }
            AIHC_Thumb_SetIdxByPos( (uiPosY - 1) * m_AHCThumb.usThumbPerRow + (uiPosX - 1), uiDecodeIdx);

            ThumbPosX = m_AHCThumb.StartX + uiPosX * m_AHCThumb.usThumbHorizontalGap + (uiPosX - 1) * TargetThumbWidth;
            ThumbPosY = m_AHCThumb.StartY + uiPosY * m_AHCThumb.usThumbVerticalGap   + (uiPosY - 1) * TargetThumbHeight;        
            
			
            if ( ubFileType == DCF_OBG_JPG ) 
            {
			#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
                MMP_DISPLAY_COLORMODE   DisplayColor;

                #ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
                AHC_Thumb_CalculateThumbnailBorder(&m_AHCThumb, k, BorderWidth, 1,
                                                          TargetThumbWidth, TargetThumbHeight, tmpThumbPosX, tmpThumbPosY,
                                                          &CurrRectWidth, &CurrRectHeight, &CurrRectStartX, &CurrRectStartY);

                AHC_Thumb_DrawThumbnailBorder(m_ulDrawingThumbOSD, RGB565_COLOR_GRAY, BorderWidth,
                                                        CurrRectWidth, CurrRectHeight, CurrRectStartX, CurrRectStartY, tmpThumbPosX, tmpThumbPosY);
                #endif
                
                if(AHC_TRUE == AHC_Thumb_GetJPGFrame(FilePathName, &TargetThumbAddr, OSDColor,
                                                        TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH)){
                    AHC_Thumb_CopyVideoFrameToOSD(m_ulDrawingThumbOSD, TargetThumbAddr, OSDColor, ubLCDstatus,
                        TargetThumbWidth, TargetThumbHeight, usTmpW, usTmpH, ThumbPosX, ThumbPosY);                                                        
                }
                else
				{
					if(1)//(m_FileDamage == NULL) 
					{
						MMP_GRAPHICS_RECT	Rect;
						Rect.usLeft		= ThumbPosX;
						Rect.usTop		= ThumbPosY;
						Rect.usWidth	= TargetThumbWidth;
						Rect.usHeight	= TargetThumbHeight;
						AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x9090/*black*/,&Rect);
					}
					else 
					{
						ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
						ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;
						AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
					}
                    //AHC_Thumb_DrawDamageVideoFrameToOSD(m_ulDrawingThumbOSD, GUI_GREEN, m_FileDamage,
                    //                                        TargetThumbWidth, TargetThumbHeight, ThumbPosX, ThumbPosY);                        
                }
			#else
                MMP_DSC_JPEG_INFO       jpeginfo;
                MMP_GRAPHICS_BUF_ATTR   jpegbufattribute;
                MMP_DISPLAY_COLORMODE   DisplayColor;
                MEMSET(&jpeginfo, 0, sizeof(MMP_DSC_JPEG_INFO));
                //MEMSET(jpeginfo.bJpegFileName, 0, DSC_MAX_FILE_NAME_SIZE);
                
                STRCPY(jpeginfo.bJpegFileName, FilePathName);
                
                jpeginfo.usJpegFileNameLength   = STRLEN(FilePathName);
                jpeginfo.ulJpegBufAddr          = 0;
                jpeginfo.ulJpegBufSize          = 0;
                jpeginfo.bDecodeThumbnail       = MMP_TRUE;
                #if (DSC_SUPPORT_BASELINE_MP_FILE)
                jpeginfo.bDecodeLargeThumb 		= MMP_TRUE;
                #endif
                jpeginfo.bValid                 = MMP_FALSE;

      			#ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
                {
                    //tmpThumbPosX = 1+k-(k/m_AHCThumb.usThumbPerRow)*m_AHCThumb.usThumbPerRow;
                    tmpThumbPosX = k-((k-1)/m_AHCThumb.usThumbPerRow)*m_AHCThumb.usThumbPerRow;
                    //tmpThumbPosY = 1+k/m_AHCThumb.usThumbPerRow;
                    tmpThumbPosY = 1+(k-1)/m_AHCThumb.usThumbPerRow;
                    
                    CurrRectWidth = TargetThumbWidth + BorderWidth + BorderWidth/2;
                    CurrRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;
                    CurrRectStartX = m_AHCThumb.StartX + (tmpThumbPosX-1)*(m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
                    CurrRectStartY = m_AHCThumb.StartY + (tmpThumbPosY-1)*(m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth;

                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetColor(m_ulDrawingThumbOSD, RGB565_COLOR_GRAY);
                    #else
                    AHC_OSDSetColor(RGB565_COLOR_GRAY);
                    #endif
			
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                                 CurrRectStartX, 
                                                 CurrRectStartY, 
                                                 CurrRectStartX + CurrRectWidth, 
                                                 CurrRectStartY + BorderWidth, 2);
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                                 CurrRectStartX-1, 
                                                 CurrRectStartY, 
                                                 CurrRectStartX+BorderWidth-1, 
                                                 CurrRectStartY + CurrRectHeight+1, 2);
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                                 CurrRectStartX + CurrRectWidth + (BorderWidth/2)-BorderWidth, 
                                                 CurrRectStartY, 
                                                 CurrRectStartX + CurrRectWidth + (BorderWidth/2), 
                                                 CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2); 
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                                 CurrRectStartX, 
                                                 CurrRectStartY + CurrRectHeight + (BorderWidth/2)-BorderWidth, 
                                                 CurrRectStartX + CurrRectWidth, 
                                                 CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2);                
                }
      			#endif
                
                if ( ( OSDColor == OSD_COLOR_RGB565 ) || ( OSDColor == OSD_COLOR_RGB888 ) ) 
                {
                    UINT16 uwBufWidth;
                    UINT16 uwBufHeight;
                    
                    if ( OSDColor == OSD_COLOR_RGB565 ) {
                        DisplayColor = MMP_DISPLAY_COLOR_RGB565;
                    }
                    else if ( OSDColor == OSD_COLOR_RGB888 ) {
                        DisplayColor = MMP_DISPLAY_COLOR_RGB888;
                    }

                    #if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
                    uwBufWidth =  ((usTmpW) >> 2) << 2;
                    uwBufHeight = ((usTmpH)>> 2) << 2;
                    #else
                    uwBufWidth =  ((usTmpW+15) >> 4) << 4;
                    uwBufHeight = ((usTmpH+15)>> 4) << 4;
                    #endif

                    error = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                                      uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                                                      uwBufHeight,//((usTmpH + 15) >> 4) << 4,
                                                      DisplayColor);

                    if( jpeginfo.usThumbHeight == 0 )
                    {
                        AIHC_Thumb_CalculateRatio( jpeginfo.usPrimaryWidth, jpeginfo.usPrimaryHeight, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);
                        
                        error = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                                          uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                                                          uwBufHeight,//((usTmpH + 15) >> 4) << 4,
                                                          DisplayColor);
                    }
                    
                    if ( error == MMP_ERR_NONE) {
                    
                        MMPS_DSC_GetJpegDispBufAttr(&jpegbufattribute);

                        	AIHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                        			(UINT8*)jpegbufattribute.ulBaseAddr,
                        			AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, uwBufWidth ),
                        			AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, uwBufHeight ),
                        			uwBufWidth,//usTmpW,
                        			uwBufHeight,//usTmpH,
                        			uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                        			OSDColor);
                    }
                    else {
                        if(m_FileDamage == NULL) {
                            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                            AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GREEN);
                            #else
                            AHC_OSDSetColor(GUI_GREEN);
                            #endif
                            AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                                ThumbPosX,
                                                ThumbPosY,
                                                ThumbPosX + TargetThumbWidth,
                                                ThumbPosY + TargetThumbHeight);
                        }
                        else {
                            ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
                            ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;

                            AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
                        }
                    }    
                }
			#endif
            }
            else if ( ( ubFileType == DCF_OBG_MOV ) ||
                      ( ubFileType == DCF_OBG_MP4 ) ||
                      ( ubFileType == DCF_OBG_AVI ) ||
                      ( ubFileType == DCF_OBG_3GP ) ||
                      ( ubFileType == DCF_OBG_WMV ) )
            {
			#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
                MMP_GRAPHICS_COLORDEPTH GraphicColorFormat;

                if(AHC_TRUE == AHC_Thumb_GetVideoFrame(FilePathName, TargetThumbAddr, OSDColor,
                                                        TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH)){
                    AHC_Thumb_CopyVideoFrameToOSD(m_ulDrawingThumbOSD, TargetThumbAddr, OSDColor, ubLCDstatus,
                        TargetThumbWidth, TargetThumbHeight, usTmpW, usTmpH, ThumbPosX, ThumbPosY);                                                        
                    
                    AHC_Thumb_CopyEmrIconToOSD(m_ulDrawingThumbOSD, gpBmpEmrIcon, ThumbPosX, ThumbPosY);
                }
                else
				{
					if(1)//(m_FileDamage == NULL) 
					{
						MMP_GRAPHICS_RECT	Rect;
						Rect.usLeft		= ThumbPosX;
						Rect.usTop		= ThumbPosY;
						Rect.usWidth	= TargetThumbWidth;
						Rect.usHeight	= TargetThumbHeight;
						AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0x9090/*black*/,&Rect);
					}
					else 
					{
						ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
						ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;
						AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
					}
                }
			#else
            	#ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
                {
                    //tmpThumbPosX = 1+k-(k/m_AHCThumb.usThumbPerRow)*m_AHCThumb.usThumbPerRow;
                    tmpThumbPosX = k-((k-1)/m_AHCThumb.usThumbPerRow)*m_AHCThumb.usThumbPerRow;
                    //tmpThumbPosY = 1+k/m_AHCThumb.usThumbPerRow;
                    tmpThumbPosY = 1+(k-1)/m_AHCThumb.usThumbPerRow;

                    //if ( (tmpThumbPosX <= m_AHCThumb.usThumbPerRow) && (tmpThumbPosY <= m_AHCThumb.usThumbPerCol) )

                    CurrRectWidth = TargetThumbWidth + BorderWidth + BorderWidth/2;
                    CurrRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;
                    CurrRectStartX = m_AHCThumb.StartX + (tmpThumbPosX-1)*(m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
                    CurrRectStartY = m_AHCThumb.StartY + (tmpThumbPosY-1)*(m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth;

                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GRAY);
                    #else
                    AHC_OSDSetColor(GUI_GRAY);
                    #endif

                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                         CurrRectStartX, 
                                         CurrRectStartY, 
                                         CurrRectStartX + CurrRectWidth, 
                                         CurrRectStartY + BorderWidth, 2);
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                         CurrRectStartX-1, 
                                         CurrRectStartY, 
                                         CurrRectStartX+BorderWidth-1, 
                                         CurrRectStartY + CurrRectHeight+1, 2);
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                         CurrRectStartX + CurrRectWidth + (BorderWidth/2)-BorderWidth, 
                                         CurrRectStartY, 
                                         CurrRectStartX + CurrRectWidth + (BorderWidth/2), 
                                         CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2); 
                    AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                         CurrRectStartX, 
                                         CurrRectStartY + CurrRectHeight + (BorderWidth/2)-BorderWidth, 
                                         CurrRectStartX + CurrRectWidth, 
                                         CurrRectStartY + CurrRectHeight + (BorderWidth/2),2);                
                }
            	#endif

                if ( ( OSDColor == OSD_COLOR_RGB565 ) || ( OSDColor == OSD_COLOR_RGB888 ) ) 
                {
                    MMP_GRAPHICS_COLORDEPTH GraphicColorFormat;
#if !(EN_SPEED_UP_VID & BROWSER_CASE)
                    MMPS_VIDEO_CONTAINER_INFO info;
#endif
                    if ( OSDColor == OSD_COLOR_RGB565 ) {
                        GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_16;
                    }
                    else if ( OSDColor == OSD_COLOR_RGB888 ) {
                        GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_24;
                    }
                    
                    //Rogers@20110825:Del for HDMI Browser -> VP
                    //MMPS_VIDPLAY_InitConfig(MMPS_VIDPLAY_GetConfiguration());
#if !(EN_SPEED_UP_VID & BROWSER_CASE)
                    error = MMPS_VIDPLAY_GetContainerInfo(FilePathName, STRLEN(FilePathName), &info); 

                    if ( error == MMP_ERR_NONE) 
#endif					
                    {
                        MMPS_VIDPLAY_SCALERCONFIG sScalerCfg;

						MMPS_VIDPLAY_SetScalerCfg(&sScalerCfg, MMP_FALSE, 0, 0, 0, 0);
                        MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, MMP_DISPLAY_ROTATE_NO_ROTATE, MMP_FALSE, NULL, &sScalerCfg);
#if !(EN_SPEED_UP_VID & BROWSER_CASE)
                        AIHC_Thumb_CalculateRatio( info.video_info[0].width, 
                                                   info.video_info[0].height, 
                                                   TargetThumbWidth, 
                                                   TargetThumbHeight, 
                                                   &usTmpW, 
                                                   &usTmpH );
#else												   
                        MMPS_VIDPLAY_SetThumbSize(TargetThumbWidth, TargetThumbHeight);
                        AHC_OS_GetTime(&t1);						
#endif						
                        error = MMPS_VIDPLAY_GetFrame(FilePathName, 
                                                      STRLEN(FilePathName), 
                                                      &usTmpW, 
                                                      &usTmpH, 
                                                      GraphicColorFormat, 
                                                      (MMP_USHORT*)TargetThumbAddr, 
                                                      &TargetThumbSize, 
                                                      MMPS_3GP_GETFRAME_FIRST);
#if (EN_SPEED_UP_VID & BROWSER_CASE)										  
                        AHC_OS_GetTime(&t2);
                        printc(FG_YELLOW("GET_FRM_END: %d %d\r\n"), t1, (t2-t1));
#endif													  
                    }

                    // Check the return value of MMPS_VIDPLAY_GetContainerInfo or MMPS_VIDPLAY_GetFrame
                    if ( error == MMP_ERR_NONE) {
                        
                        AHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                                          (UINT8*)TargetThumbAddr,
                                          AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                          AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                          usTmpW,
                                          usTmpH,
                                          OSDColor,ubLCDstatus);
                        if(gpBmpEmrIcon){
                            if(AHC_UF_GetDB() == DCF_DB_TYPE_3RD_DB)
                                AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, gpBmpEmrIcon, ThumbPosX, ThumbPosY);
                        }

                    }
                    else 
                    {
                        printc("error MMPS_VIDPLAY_GetFrame : 0x%x\r\n",error);
                        if(error==MMP_VIDPSR_ERR_CONTENT_CORRUPT)
                        {
                            printc(FG_RED("MMP_VIDPSR_ERR_CONTENT_CORRUPT BREAK\r\n"));
                            break;
                        }

                        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                        AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GREEN);
                        #else
                        AHC_OSDSetColor(GUI_GREEN);
                        #endif
                            
                        if(m_FileDamage == NULL) {
                            AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                                ThumbPosX,
                                                ThumbPosY,
                                                ThumbPosX + TargetThumbWidth,
                                                ThumbPosY + TargetThumbHeight);
                        }
                        else {
                            ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
                            ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;

                            AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
                            
                            #ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
                            {
                                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                                AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GRAY);
                                #else
                                AHC_OSDSetColor(GUI_GRAY);
                                #endif
                                
								
                                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD,
                                                     CurrRectStartX,
                                                     CurrRectStartY,
                                                     CurrRectStartX + CurrRectWidth,
                                                     CurrRectStartY + BorderWidth, 2);
                                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD,
                                                     CurrRectStartX-1,
                                                     CurrRectStartY,
                                                     CurrRectStartX+BorderWidth-1,
                                                     CurrRectStartY + CurrRectHeight+1, 2);
                                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD,
                                                     CurrRectStartX + CurrRectWidth + (BorderWidth/2)-BorderWidth, 
                                                     CurrRectStartY,
                                                     CurrRectStartX + CurrRectWidth + (BorderWidth/2),
                                                     CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2); 
                                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD,
                                                     CurrRectStartX,
                                                     CurrRectStartY + CurrRectHeight + (BorderWidth/2)-BorderWidth, 
                                                     CurrRectStartX + CurrRectWidth,
                                                     CurrRectStartY + CurrRectHeight + (BorderWidth/2),2);
                            }
                            #endif
                        }
                    }
                }
			#endif
            }
            else if ( ( ubFileType == DCF_OBG_MP3 ) ||
                      ( ubFileType == DCF_OBG_WAV ) ||
                      ( ubFileType == DCF_OBG_OGG ) ||
                      ( ubFileType == DCF_OBG_WMA ) )
            {
                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_BLUE);
                #else
                AHC_OSDSetColor(GUI_BLUE);
                #endif
                AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                    ThumbPosX,
                                    ThumbPosY,
                                    ThumbPosX + TargetThumbWidth,
                                    ThumbPosY + TargetThumbHeight);
            }
            else 
            {
                //TBD, Not supported file
                if(m_FileDamage == NULL) {
                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GREEN);
                    #else
                    AHC_OSDSetColor(GUI_GREEN);
                    #endif

                    AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                        ThumbPosX,
                                        ThumbPosY,
                                        ThumbPosX + TargetThumbWidth,
                                        ThumbPosY + TargetThumbHeight);
                }
                else {
                    ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
                    ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;

                    AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
                }	
            }
            


            if(uiDecodeIdx == uiMaxIdx)
                break;
            uiDecodeIdx++;

            k++;

        } while(k <= uiThumbPerPage);
        
        #ifdef WMSG_SHOW_WAIT
        AHC_WMSG_Draw(AHC_FALSE, WMSG_NONE, 0);
        #endif
        
        //backup for double buffer display
        BEGIN_LAYER(m_ulDisplayThumbOSD);
        AHC_OSDDuplicateBuffer(m_ulDrawingThumbOSD, m_ulDisplayThumbOSD);
        END_LAYER(m_ulDisplayThumbOSD);
    }

    //restore color
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
    AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
    #else
    AHC_OSDSetColor(CurrentGUIColor);
    AHC_OSDSetBkColor(CurrentBkColor);
    #endif

    END_LAYER(m_ulDrawingThumbOSD);
    ThumbIndex = m_AHCThumb.uiIdxMap[ (uiCurIdx % (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol)) ];

    #if (THUMBNAIL_BUFFER_NUMBER == 2)
    AIHC_Thumb_DrawFocus(ThumbIndex, AHC_TRUE);
    #else 
    AIHC_Thumb_DrawFocus(ThumbIndex, AHC_FALSE);
    #endif
    
    m_AHCThumb.uwCurIdx = ThumbIndex;

    AHC_Thumb_ProtectKey();

    AIHC_ThumbFree((void*)TargetThumbAddr);

    BEGIN_LAYER(m_ulDrawingThumbOSD);

    //restore color
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
    AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
    #else
    AHC_OSDSetColor(CurrentGUIColor);
    AHC_OSDSetBkColor(CurrentBkColor);
    #endif
        
    m_uiCurPage = uiCurPage;

    m_uiMaxIdx  = uiMaxIdx;

    AIHC_DrawReservedOSD(AHC_FALSE);

    END_LAYER(m_ulDrawingThumbOSD);

    return AHC_TRUE;

}

AHC_BOOL AHC_Thumb_Quick_DrawLockOrUnlock(void)
{
    UINT32                      uiCurIdx;
    UINT32                      uiMaxIdx;
    UINT32                      uiThumbPerPage;
    UINT32                      uiPosY, uiPosX;
    UINT32						uiDecodeIdx;
    UINT8 						ubFileType;
    INT8                        FilePathName[MAX_FILE_NAME_SIZE];
    UINT16                      OSDWidth,OSDHeight,OSDColor,TargetThumbWidth,TargetThumbHeight;
    UINT16                      usTmpW,usTmpH;
    UINT32                      OSDAddr;
    UINT32                      TargetThumbAddr,ThumbPosX,ThumbPosY;	
    UINT16                      ThumbIndex,CurPosCurPage;
    MMP_ULONG                   TargetThumbSize;
    #ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
    UINT32 				        BorderWidth = 4;
    UINT32						CurrRectWidth;
    UINT32						CurrRectHeight;
    UINT32						CurrRectStartX;
    UINT32						CurrRectStartY;
    UINT32						tmpThumbPosX;
    UINT32						tmpThumbPosY;
    #endif
    MMP_ERR error;
    GUI_COLOR                   CurrentGUIColor,CurrentBkColor;
    MMP_UBYTE					ubLCDstatus = 0;
    AHC_DISPLAY_OUTPUTPANEL  	OutputDevice;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }
    
    BEGIN_LAYER(m_ulDrawingThumbOSD);
    AIHC_DrawReservedOSD(AHC_TRUE);
    printc("@@@@@@@ AHC_Thumb_Draw_CurFile_AddLockOrUnlock \r\n");
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    CurrentGUIColor = AHC_OSDGetColor(m_ulDrawingThumbOSD);
    CurrentBkColor  = AHC_OSDGetBkColor(m_ulDrawingThumbOSD);
    #else
    CurrentGUIColor = AHC_OSDGetColor();
    CurrentBkColor  = AHC_OSDGetBkColor();
    #endif

    AHC_OSDGetBufferAttr(m_ulDrawingThumbOSD, &OSDWidth, &OSDHeight, &OSDColor, &OSDAddr);
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    {
        UINT16  ulOSDTemp = 0;

        //Swap width/height. Rotate OSD by GUI.
        ulOSDTemp = OSDWidth;
        OSDWidth = OSDHeight;
        OSDHeight = ulOSDTemp;
    }
#endif
    if( AHC_FALSE == AHC_UF_GetTotalFileCount(&uiMaxIdx) )
    {
        if(AHC_UF_IsDBMounted()) {
            AIHC_DrawReservedOSD(AHC_FALSE);
            END_LAYER(m_ulDrawingThumbOSD);

            return AHC_FALSE;
        }
    }
    
    /*
    There is no dcf file in the db.
    
    */
    if ( uiMaxIdx == 0 ) {
        //No DCF obj
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        #else
        AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_AHCThumb.ulBkColor);
        #endif

        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, 0, 0, OSDWidth, OSDHeight);

        AHC_OSDSetCurrentDisplay(m_ulDrawingThumbOSD);
        AHC_OSDSetActive(m_ulDrawingThumbOSD,AHC_TRUE);

        AHC_OSDRefresh();
        
        //restore color
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
        #else
        AHC_OSDSetColor(CurrentGUIColor);
        AHC_OSDSetBkColor(CurrentBkColor);
        #endif
    
        AIHC_DrawReservedOSD(AHC_FALSE);
        
        m_uiCurPage = -1;

        m_uiMaxIdx  = -1;

        END_LAYER(m_ulDrawingThumbOSD);
        
        return AHC_TRUE;
    }

    AHC_GetDisplayOutputDev(&OutputDevice);

    if(OutputDevice == MMP_DISPLAY_SEL_MAIN_LCD) {
    
        #define HORIZONTAL_RATIO    (4)
        #define VERTICAL_RATIO      (3)

        #if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
        { // 4:3
        UINT32 uiX;
        UINT32 uiY;
        
        uiX =   ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) 
                / (HORIZONTAL_RATIO*m_AHCThumb.usThumbPerRow);
        
        uiY =   ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) 
                / (VERTICAL_RATIO*m_AHCThumb.usThumbPerCol);
                            
        if(uiX > uiY){
            TargetThumbWidth    = HORIZONTAL_RATIO*uiY;
            TargetThumbHeight   = VERTICAL_RATIO*uiY;
        }else{
            TargetThumbWidth    = HORIZONTAL_RATIO*uiX;
            TargetThumbHeight   = VERTICAL_RATIO*uiX;
        }	    
        }
        #else
        
        {
        TargetThumbWidth  = ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) 
                            / m_AHCThumb.usThumbPerRow;
                            
        TargetThumbHeight = ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) 
                            / m_AHCThumb.usThumbPerCol;
        }
        #endif
    }
    else {
        TargetThumbWidth  = ( m_AHCThumb.DisplayWidth  - (m_AHCThumb.usThumbPerRow + 1) * (m_AHCThumb.usThumbHorizontalGap) ) 
                            / m_AHCThumb.usThumbPerRow;
                            
        TargetThumbHeight = ( m_AHCThumb.DisplayHeight - (m_AHCThumb.usThumbPerCol + 1) * (m_AHCThumb.usThumbVerticalGap  ) ) 
                            / m_AHCThumb.usThumbPerCol;	

    }	
    
    m_AHCThumb.uwTargetHorizontal   = TargetThumbWidth;
    m_AHCThumb.uwTargetVertical     = TargetThumbHeight;
    
    AIHC_Thumb_CalculateRatio( 160, 120, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);

    TargetThumbAddr   = (UINT32)AIHC_ThumbMalloc(TargetThumbWidth * TargetThumbHeight * 4);//0x1700000 + TargetThumbWidth * TargetThumbHeight * 4;  

    // the cuurent dcf index in the db.
    AHC_UF_GetCurrentIndex(&uiCurIdx);
    
    // the boundary check of index.
    if ( uiCurIdx >= (uiMaxIdx - 1) ){
        uiCurIdx = (uiMaxIdx - 1);
    }
    
    // the max index in the db
    uiMaxIdx -= 1;   
    
    uiDecodeIdx = uiCurIdx;//uiCurPage * uiThumbPerPage;
    /*
        the boundary check of index.
    */
    if ( uiDecodeIdx > uiMaxIdx ) {

        uiThumbPerPage = (uiMaxIdx + 1) % (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
        
        if(uiThumbPerPage == 0)
            uiThumbPerPage = (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
            
        uiDecodeIdx = uiMaxIdx;
    }
    
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        #else
        AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_AHCThumb.ulBkColor);
        #endif

        AHC_Thumb_GetCurPosCurPage(&CurPosCurPage);
            
        if ( ((CurPosCurPage+1) % m_AHCThumb.usThumbPerRow) == 0 ) {
            uiPosY = CurPosCurPage / m_AHCThumb.usThumbPerRow ;
            uiPosX = m_AHCThumb.usThumbPerRow-1;
        }
        else {
            uiPosY = CurPosCurPage / m_AHCThumb.usThumbPerRow ;
            uiPosX = CurPosCurPage % m_AHCThumb.usThumbPerRow;
        }
        printc("CurPosCurPage %d uiPosX %d uiPosY %d\r\n",CurPosCurPage,uiPosX,uiPosY);
        AIHC_Thumb_CalculateRatio( 160, 120, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);
        
        /*
        Get the file information.
        */
        MEMSET(FilePathName, 0, sizeof(FilePathName));
        AHC_UF_GetFileTypebyIndex(uiDecodeIdx, &ubFileType);
        AHC_UF_GetFilePathNamebyIndex(uiDecodeIdx, FilePathName);

        ThumbPosX = m_AHCThumb.StartX + (uiPosX+1) * m_AHCThumb.usThumbHorizontalGap + (uiPosX ) * TargetThumbWidth;
        ThumbPosY = m_AHCThumb.StartY + (uiPosY+1) * m_AHCThumb.usThumbVerticalGap   + (uiPosY ) * TargetThumbHeight;        

        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, ThumbPosX, ThumbPosY,ThumbPosX+TargetThumbWidth, ThumbPosY+TargetThumbHeight);
        
        if ( ubFileType == DCF_OBG_JPG ) 
        {
            MMP_DSC_JPEG_INFO       jpeginfo;
            MMP_GRAPHICS_BUF_ATTR   jpegbufattribute;
            MMP_DISPLAY_COLORMODE   DisplayColor;
            
            MEMSET(jpeginfo.bJpegFileName, 0, DSC_MAX_FILE_NAME_SIZE);
            
            STRCPY(jpeginfo.bJpegFileName, FilePathName);
            
            jpeginfo.usJpegFileNameLength   = STRLEN(FilePathName);
            jpeginfo.ulJpegBufAddr          = 0;
            jpeginfo.ulJpegBufSize          = 0;
            jpeginfo.bDecodeThumbnail       = MMP_TRUE;
            #if (DSC_SUPPORT_BASELINE_MP_FILE)
            jpeginfo.bDecodeLargeThumb 		= MMP_TRUE;
            #endif
            jpeginfo.bValid                 = MMP_FALSE;

#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
#ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
                AHC_Thumb_CalculateThumbnailBorder(&m_AHCThumb, CurPosCurPage, BorderWidth, 0,
                                                          TargetThumbWidth, TargetThumbHeight, tmpThumbPosX, tmpThumbPosY,
                                                          &CurrRectWidth, &CurrRectHeight, &CurrRectStartX, &CurrRectStartY);

                AHC_Thumb_DrawThumbnailBorder(m_ulDrawingThumbOSD, RGB565_COLOR_GRAY, BorderWidth,
                                                        CurrRectWidth, CurrRectHeight, CurrRectStartX, CurrRectStartY, tmpThumbPosX, tmpThumbPosY);
#endif            
#else
            #ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
            {
                tmpThumbPosX = CurPosCurPage-((CurPosCurPage)/m_AHCThumb.usThumbPerRow)*m_AHCThumb.usThumbPerRow;
                tmpThumbPosY = 1+(CurPosCurPage)/m_AHCThumb.usThumbPerRow;
                
                CurrRectWidth = TargetThumbWidth + BorderWidth + BorderWidth/2;
                CurrRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;
                CurrRectStartX = m_AHCThumb.StartX + tmpThumbPosX*(m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
                CurrRectStartY = m_AHCThumb.StartY + tmpThumbPosY*(m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth;

                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(m_ulDrawingThumbOSD, RGB565_COLOR_GRAY);
                #else
                AHC_OSDSetColor(RGB565_COLOR_GRAY);
                #endif

				#if CCIR656_OUTPUT_ENABLE
				{
				    MMP_GRAPHICS_RECT          	Rect;
				    BorderWidth *=2;
				    Rect.usLeft  = CurrRectStartX;
				    Rect.usTop=CurrRectStartY-BorderWidth;
				    Rect.usWidth = CurrRectWidth;
				    Rect.usHeight = BorderWidth + 1;
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

				    Rect.usLeft = CurrRectStartX-BorderWidth;
				    Rect.usTop  =CurrRectStartY-BorderWidth;
				    Rect.usWidth = BorderWidth;
				    Rect.usHeight = CurrRectHeight+2*BorderWidth;
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

				    Rect.usLeft =CurrRectStartX + CurrRectWidth;
				    Rect.usTop = CurrRectStartY -BorderWidth;
				    Rect.usWidth =   BorderWidth;
				    Rect.usHeight = CurrRectHeight + 2*(BorderWidth);
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

				    Rect.usLeft =CurrRectStartX;
				    Rect.usTop = CurrRectStartY + CurrRectHeight;
				    Rect.usWidth = CurrRectWidth;
				    Rect.usHeight =   BorderWidth;
				    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

				}

				#else
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                             CurrRectStartX, 
                                             CurrRectStartY, 
                                             CurrRectStartX + CurrRectWidth, 
                                             CurrRectStartY + BorderWidth, 2);
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                             CurrRectStartX-1, 
                                             CurrRectStartY, 
                                             CurrRectStartX+BorderWidth-1, 
                                             CurrRectStartY + CurrRectHeight+1, 2);
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                             CurrRectStartX + CurrRectWidth + (BorderWidth/2)-BorderWidth, 
                                             CurrRectStartY, 
                                             CurrRectStartX + CurrRectWidth + (BorderWidth/2), 
                                             CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2); 
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                             CurrRectStartX, 
                                             CurrRectStartY + CurrRectHeight + (BorderWidth/2)-BorderWidth, 
                                             CurrRectStartX + CurrRectWidth, 
                                             CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2);                
				#endif
            }
            #endif
            
#endif
            if ( ( OSDColor == OSD_COLOR_RGB565 ) || ( OSDColor == OSD_COLOR_RGB888 ) ) 
            {
                UINT16 uwBufWidth;
                UINT16 uwBufHeight;
                
                if ( OSDColor == OSD_COLOR_RGB565 ) {
                    DisplayColor = MMP_DISPLAY_COLOR_RGB565;
                }
                else if ( OSDColor == OSD_COLOR_RGB888 ) {
                    DisplayColor = MMP_DISPLAY_COLOR_RGB888;
                }

                #if (AHC_BROWSER_THUMBNAIL_STYLE == AHC_BROWSER_THUMBNAIL_STYLE_1)
                uwBufWidth =  ((usTmpW) >> 2) << 2;
                uwBufHeight = ((usTmpH)>> 2) << 2;
                #else
                uwBufWidth =  ((usTmpW+15) >> 4) << 4;
                uwBufHeight = ((usTmpH+15)>> 4) << 4;
                #endif

                error = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                                  uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                                                  uwBufHeight,//((usTmpH + 15) >> 4) << 4,
                                                  DisplayColor);

                if( jpeginfo.usThumbHeight == 0 )
                {
                    AIHC_Thumb_CalculateRatio( jpeginfo.usPrimaryWidth, jpeginfo.usPrimaryHeight, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);
                    
                    error = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                                      uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                                                      uwBufHeight,//((usTmpH + 15) >> 4) << 4,
                                                      DisplayColor);
                }
                
                if ( error == MMP_ERR_NONE) {
                
                    MMPS_DSC_GetJpegDispBufAttr(&jpegbufattribute);

                    AIHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                                      (UINT8*)jpegbufattribute.ulBaseAddr,
                                      AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                      AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                      uwBufWidth,//usTmpW,
                                      uwBufHeight,//usTmpH,
                                      uwBufWidth,//((usTmpW + 15) >> 4) << 4,
                                      OSDColor);
                }
            }
        }
        else if ( ( ubFileType == DCF_OBG_MOV ) ||
                  ( ubFileType == DCF_OBG_MP4 ) ||
                  ( ubFileType == DCF_OBG_AVI ) ||
                  ( ubFileType == DCF_OBG_3GP ) ||
                  ( ubFileType == DCF_OBG_WMV ) )
        {

#if defined(CCIR656_OUTPUT_ENABLE)&&(CCIR656_OUTPUT_ENABLE)
#ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
                AHC_Thumb_CalculateThumbnailBorder(&m_AHCThumb, k, BorderWidth, 0,
                                                          TargetThumbWidth, TargetThumbHeight, tmpThumbPosX, tmpThumbPosY,
                                                          &CurrRectWidth, &CurrRectHeight, &CurrRectStartX, &CurrRectStartY);

                AHC_Thumb_DrawThumbnailBorder(m_ulDrawingThumbOSD, GUI_GRAY, BorderWidth,
                                                        CurrRectWidth, CurrRectHeight, CurrRectStartX, CurrRectStartY, tmpThumbPosX, tmpThumbPosY);
#endif
#else        
            #ifndef AHC_BROWSER_THUMBNAIL_NO_GRAY_BORDER
            {
                tmpThumbPosX = CurPosCurPage-(CurPosCurPage/m_AHCThumb.usThumbPerRow)*m_AHCThumb.usThumbPerRow;
                tmpThumbPosY = 1+CurPosCurPage/m_AHCThumb.usThumbPerRow;

                CurrRectWidth = TargetThumbWidth + BorderWidth + BorderWidth/2;
                CurrRectHeight = TargetThumbHeight + BorderWidth + BorderWidth/2;
                CurrRectStartX = m_AHCThumb.StartX + tmpThumbPosX*(m_AHCThumb.usThumbHorizontalGap + TargetThumbWidth) + m_AHCThumb.usThumbHorizontalGap - BorderWidth;
                CurrRectStartY = m_AHCThumb.StartY + tmpThumbPosY*(m_AHCThumb.usThumbVerticalGap + TargetThumbHeight) + m_AHCThumb.usThumbVerticalGap - BorderWidth;

                #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GRAY);
                #else
                AHC_OSDSetColor(GUI_GRAY);
                #endif

#if CCIR656_OUTPUT_ENABLE
			{
			    MMP_GRAPHICS_RECT          	Rect;
			    BorderWidth *=2;
			    Rect.usLeft  = CurrRectStartX;
			    Rect.usTop=CurrRectStartY-BorderWidth;
			    Rect.usWidth = CurrRectWidth;
			    Rect.usHeight = BorderWidth + 1;
			    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			    Rect.usLeft = CurrRectStartX-BorderWidth;
			    Rect.usTop  =CurrRectStartY-BorderWidth;
			    Rect.usWidth = BorderWidth;
			    Rect.usHeight = CurrRectHeight+2*BorderWidth;
			    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			    Rect.usLeft =CurrRectStartX + CurrRectWidth;
			    Rect.usTop = CurrRectStartY -BorderWidth;
			    Rect.usWidth =   BorderWidth;
			    Rect.usHeight = CurrRectHeight + 2*(BorderWidth);
			    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			    Rect.usLeft =CurrRectStartX;
			    Rect.usTop = CurrRectStartY + CurrRectHeight;
			    Rect.usWidth = CurrRectWidth;
			    Rect.usHeight =   BorderWidth;
			    AHC_Thumb_DrawRectToCCIR656OutputBufferRect((UINT32)m_ulCCIR656buf, 720, 480, 0xE0E0/*black*/,&Rect);

			}

#else
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                     CurrRectStartX, 
                                     CurrRectStartY, 
                                     CurrRectStartX + CurrRectWidth, 
                                     CurrRectStartY + BorderWidth, 2);
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                     CurrRectStartX-1, 
                                     CurrRectStartY, 
                                     CurrRectStartX+BorderWidth-1, 
                                     CurrRectStartY + CurrRectHeight+1, 2);
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                     CurrRectStartX + CurrRectWidth + (BorderWidth/2)-BorderWidth, 
                                     CurrRectStartY, 
                                     CurrRectStartX + CurrRectWidth + (BorderWidth/2), 
                                     CurrRectStartY + CurrRectHeight + (BorderWidth/2), 2); 
                AHC_OSDDrawFillRoundedRect( m_ulDrawingThumbOSD, 
                                     CurrRectStartX, 
                                     CurrRectStartY + CurrRectHeight + (BorderWidth/2)-BorderWidth, 
                                     CurrRectStartX + CurrRectWidth, 
                                     CurrRectStartY + CurrRectHeight + (BorderWidth/2),2);
#endif
            }
            #endif
#endif
            if ( ( OSDColor == OSD_COLOR_RGB565 ) || ( OSDColor == OSD_COLOR_RGB888 ) ) 
            {
                MMP_GRAPHICS_COLORDEPTH GraphicColorFormat;
                MMPS_VIDEO_CONTAINER_INFO info;

                if ( OSDColor == OSD_COLOR_RGB565 ) {
                    GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_16;
                }
                else if ( OSDColor == OSD_COLOR_RGB888 ) {
                    GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_24;
                }                    
        
                error = MMPS_VIDPLAY_GetContainerInfo(FilePathName, STRLEN(FilePathName), &info);

                if ( error == MMP_ERR_NONE) {

                    MMPS_VIDPLAY_SCALERCONFIG sScalerCfg;

					MMPS_VIDPLAY_SetScalerCfg(&sScalerCfg, MMP_FALSE, 0, 0, 0, 0);
                    MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, MMP_DISPLAY_ROTATE_NO_ROTATE, MMP_FALSE, NULL, &sScalerCfg);

                    AIHC_Thumb_CalculateRatio( info.video_info[0].width, 
                                               info.video_info[0].height, 
                                               TargetThumbWidth, 
                                               TargetThumbHeight, 
                                               &usTmpW, 
                                               &usTmpH );

                    error = MMPS_VIDPLAY_GetFrame(FilePathName, 
                                                  STRLEN(FilePathName), 
                                                  &usTmpW, 
                                                  &usTmpH, 
                                                  GraphicColorFormat, 
                                                  (MMP_USHORT*)TargetThumbAddr, 
                                                  &TargetThumbSize, 
                                                  MMPS_3GP_GETFRAME_FIRST);
                }
                                  
                if ( error == MMP_ERR_NONE) {
                    AHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                                      (UINT8*)TargetThumbAddr,
                                      AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                      AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                      usTmpW,
                                      usTmpH,
                                      OSDColor,ubLCDstatus);
                }                    
            }
        }
        else if ( ( ubFileType == DCF_OBG_MP3 ) ||
                  ( ubFileType == DCF_OBG_WAV ) ||
                  ( ubFileType == DCF_OBG_OGG ) ||
                  ( ubFileType == DCF_OBG_WMA ) )
        {
            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_BLUE);
            #else
            AHC_OSDSetColor(GUI_BLUE);
            #endif
            AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                ThumbPosX,
                                ThumbPosY,
                                ThumbPosX + TargetThumbWidth,
                                ThumbPosY + TargetThumbHeight);
        }            

    //backup for double buffer display
    BEGIN_LAYER(m_ulDisplayThumbOSD);
    AHC_OSDDuplicateBuffer(m_ulDrawingThumbOSD, m_ulDisplayThumbOSD);
    END_LAYER(m_ulDisplayThumbOSD);

    //restore color
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
    AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
    #else
    AHC_OSDSetColor(CurrentGUIColor);
    AHC_OSDSetBkColor(CurrentBkColor);
    #endif

    END_LAYER(m_ulDrawingThumbOSD);
    ThumbIndex = m_AHCThumb.uiIdxMap[ (uiCurIdx % (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol)) ];

    #if (THUMBNAIL_BUFFER_NUMBER == 2)
    AIHC_Thumb_DrawFocus(ThumbIndex, AHC_TRUE);
    #else
    AIHC_Thumb_DrawFocus(ThumbIndex, AHC_FALSE);
    #endif
    
    {
        AHC_BOOL bReadOnly = AHC_FALSE;
        AHC_BOOL bCharLock = AHC_FALSE;
        AHC_UF_IsCharLockbyIndex(uiDecodeIdx, &bCharLock);
        AHC_UF_IsReadOnlybyIndex(uiDecodeIdx, &bReadOnly);
        if (bReadOnly == AHC_TRUE)
        {
            AIHC_DrawProtectKey(uiDecodeIdx, AHC_PROTECT_MENU);
        }
        else if (bCharLock == AHC_TRUE)
        {
            AIHC_DrawProtectKey(uiDecodeIdx, AHC_PROTECT_G);
        }
    }
    
    AIHC_ThumbFree((void*)TargetThumbAddr);

    BEGIN_LAYER(m_ulDrawingThumbOSD);

    //restore color
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
    AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
    #else
    AHC_OSDSetColor(CurrentGUIColor);
    AHC_OSDSetBkColor(CurrentBkColor);
    #endif	    

    AIHC_DrawReservedOSD(AHC_FALSE);

    END_LAYER(m_ulDrawingThumbOSD);

    return AHC_TRUE;
}

AHC_BOOL AHC_DrawThumbNail(void)
{
    UINT32                      CurDcfObjIdx,MaxDcfObjIdx;
    char                        FilePathName[MAX_FILE_NAME_SIZE];
    UINT8                       ubFileType;
    UINT16                      OSDWidth,OSDHeight,OSDColor,TargetThumbWidth,TargetThumbHeight;
    UINT16                      usTmpW,usTmpH;
    UINT32                      OSDAddr;
    UINT32                      TargetThumbAddr,ThumbPosX,ThumbPosY;	
    MMP_ULONG                   TargetThumbSize;
    MMP_ERR                     error;
    MMP_DSC_JPEG_INFO           jpeginfo;
    MMP_GRAPHICS_BUF_ATTR       jpegbufattribute;
    MMP_DISPLAY_COLORMODE       DisplayColor;
    GUI_COLOR                   CurrentGUIColor,CurrentBkColor;
    MMP_UBYTE					ubLCDstatus = 0;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }

    BEGIN_LAYER(m_ulDrawingThumbOSD);

    AIHC_DrawReservedOSD(AHC_TRUE);
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    CurrentGUIColor = AHC_OSDGetColor(m_ulDrawingThumbOSD);
    CurrentBkColor  = AHC_OSDGetBkColor(m_ulDrawingThumbOSD);
    #else
    CurrentGUIColor = AHC_OSDGetColor();
    CurrentBkColor  = AHC_OSDGetBkColor();
    #endif
    
    AHC_OSDGetBufferAttr(m_ulDrawingThumbOSD,&OSDWidth,&OSDHeight,&OSDColor,&OSDAddr);
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    {
        UINT16  ulOSDTemp = 0;

        //Swap width/height. Rotate OSD by GUI.
        ulOSDTemp = OSDWidth;
        OSDWidth = OSDHeight;
        OSDHeight = ulOSDTemp;
    }
#endif    
    if( AHC_FALSE == AHC_UF_GetTotalFileCount(&MaxDcfObjIdx) )
    {
        if(AHC_UF_IsDBMounted()) {
            AIHC_DrawReservedOSD(AHC_FALSE);
            END_LAYER(m_ulDrawingThumbOSD);

            return AHC_FALSE;
        }
    }
    
    if ( MaxDcfObjIdx == 0 ) {
        //No DCF obj
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
        #else
        AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
        AHC_OSDSetColor(m_AHCThumb.ulBkColor);
        #endif
        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, 0, 0, OSDWidth, OSDHeight);
        
        AHC_OSDSetCurrentDisplay(m_ulDrawingThumbOSD);
        AHC_OSDSetActive(m_ulDrawingThumbOSD,AHC_TRUE);
        
        AHC_OSDRefresh();
        
        //restore color
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
        AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
        #else
        AHC_OSDSetColor(CurrentGUIColor);
        AHC_OSDSetBkColor(CurrentBkColor);
        #endif
            
        AIHC_DrawReservedOSD(AHC_FALSE);
        END_LAYER(m_ulDrawingThumbOSD);

        return AHC_TRUE;
    }
    
    TargetThumbWidth  = OSDWidth;
    TargetThumbHeight = OSDHeight;
    
    AIHC_Thumb_CalculateRatio( 160, 120, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);

    TargetThumbAddr   = (UINT32)AIHC_ThumbMalloc(TargetThumbWidth * TargetThumbHeight * 4);//0x1700000 + TargetThumbWidth * TargetThumbHeight * 4;
    
    AHC_UF_GetCurrentIndex(&CurDcfObjIdx);
    
    if ( CurDcfObjIdx >= (MaxDcfObjIdx - 1) )
        CurDcfObjIdx = (MaxDcfObjIdx - 1);
        
    MaxDcfObjIdx -= 1;


    //Redraw the thumbnail only if not the same page or some DCF items are removed
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetBkColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
    AHC_OSDSetColor(m_ulDrawingThumbOSD, m_AHCThumb.ulBkColor);
    #else
    AHC_OSDSetBkColor(m_AHCThumb.ulBkColor);
    AHC_OSDSetColor(m_AHCThumb.ulBkColor);
    #endif
    AHC_OSDDrawFillRect(m_ulDrawingThumbOSD, 0, 0, OSDWidth, OSDHeight);

    AIHC_Thumb_CalculateRatio( OSDWidth, OSDHeight, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);

    MEMSET(FilePathName, 0, sizeof(FilePathName));
    AHC_UF_GetFilePathNamebyIndex(CurDcfObjIdx, FilePathName);
    AHC_UF_GetFileTypebyIndex(CurDcfObjIdx, &ubFileType); 
    
    ThumbPosX = 0;            
    ThumbPosY = 0;
    
    if ( ubFileType == DCF_OBG_JPG ) 
    {
        MEMSET(jpeginfo.bJpegFileName, 0, DSC_MAX_FILE_NAME_SIZE);
        
        STRCPY(jpeginfo.bJpegFileName, FilePathName);
        
        jpeginfo.usJpegFileNameLength   = STRLEN(FilePathName);
        jpeginfo.ulJpegBufAddr          = 0;
        jpeginfo.ulJpegBufSize          = 0;
        jpeginfo.bDecodeThumbnail       = MMP_TRUE;
        #if (DSC_SUPPORT_BASELINE_MP_FILE)
        jpeginfo.bDecodeLargeThumb 		= MMP_TRUE;
        #endif
        jpeginfo.bValid                 = MMP_FALSE;
        
        if ( ( OSDColor == OSD_COLOR_RGB565 ) || ( OSDColor == OSD_COLOR_RGB888 ) ) 
        {
            if ( OSDColor == OSD_COLOR_RGB565 ) {
                DisplayColor = MMP_DISPLAY_COLOR_RGB565;
            }
            else if ( OSDColor == OSD_COLOR_RGB888 ) {
                DisplayColor = MMP_DISPLAY_COLOR_RGB888;
            }

            error = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                            ((usTmpW + 15) >> 4) << 4,
                                            ((usTmpH + 15) >> 4) << 4,
                                            DisplayColor);
            
            if( jpeginfo.usThumbHeight == 0 )
            {
                AIHC_Thumb_CalculateRatio( jpeginfo.usPrimaryWidth, jpeginfo.usPrimaryHeight, TargetThumbWidth, TargetThumbHeight, &usTmpW, &usTmpH);
                
                error = MMPS_DSC_DecodeThumbFirst(&jpeginfo, 
                                                ((usTmpW + 15) >> 4) << 4,
                                                ((usTmpH + 15) >> 4) << 4,
                                                DisplayColor);
            }
            
            if ( error == MMP_ERR_NONE) {

                MMPS_DSC_GetJpegDispBufAttr(&jpegbufattribute);
                
                AIHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                                  (UINT8*)jpegbufattribute.ulBaseAddr,
                                  AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                  AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                  usTmpW,
                                  usTmpH,
                                  ((usTmpW + 15) >> 4) << 4,
                                  OSDColor);

            }
            else {
                printc("error: AHC_DrawThumbNail : 0x%x, \r\n",error);
                if(m_FileDamage == NULL) {
                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GREEN);
                    #else
                    AHC_OSDSetColor(GUI_GREEN);
                    #endif

                    AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                      ThumbPosX,
                                      ThumbPosY,
                                        ThumbPosX + TargetThumbWidth,
                                        ThumbPosY + TargetThumbHeight);
                }
                else {
                    ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
                    ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;

                    AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
                }
            }    
        }
    }
    else if ( ( ubFileType == DCF_OBG_MOV ) ||
              ( ubFileType == DCF_OBG_MP4 ) ||
              ( ubFileType == DCF_OBG_AVI ) ||
              ( ubFileType == DCF_OBG_3GP ) ||
              ( ubFileType == DCF_OBG_WMV ) )
    {
        if ( ( OSDColor == OSD_COLOR_RGB565 ) || ( OSDColor == OSD_COLOR_RGB888 ) ) 
        {
            MMP_GRAPHICS_COLORDEPTH    GraphicColorFormat;
            MMPS_VIDEO_CONTAINER_INFO info;

            if ( OSDColor == OSD_COLOR_RGB565 ) {
                GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_16;
            }
            else if ( OSDColor == OSD_COLOR_RGB888 ) {
                GraphicColorFormat = MMP_GRAPHICS_COLORDEPTH_24;
            }
            
            //Rogers@20110825:Del for HDMI Browser -> VP
            //MMPS_VIDPLAY_InitConfig(MMPS_VIDPLAY_GetConfiguration());

            error = MMPS_VIDPLAY_GetContainerInfo(FilePathName, STRLEN(FilePathName), &info);

            if (error == MMP_ERR_NONE) { 

                MMPS_VIDPLAY_SCALERCONFIG sScalerCfg;

				MMPS_VIDPLAY_SetScalerCfg(&sScalerCfg, MMP_FALSE, 0, 0, 0, 0);
                MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, MMP_DISPLAY_ROTATE_NO_ROTATE, MMP_FALSE, NULL, &sScalerCfg);

                AIHC_Thumb_CalculateRatio( info.video_info[0].width, 
                                           info.video_info[0].height, 
                                           TargetThumbWidth, 
                                           TargetThumbHeight, 
                                           &usTmpW, 
                                           &usTmpH );

                error = MMPS_VIDPLAY_GetFrame(FilePathName, 
                                              STRLEN(FilePathName), 
                                              &usTmpW, 
                                              &usTmpH, 
                                              GraphicColorFormat, 
                                              (MMP_USHORT*)TargetThumbAddr, 
                                              &TargetThumbSize, 
                                              MMPS_3GP_GETFRAME_FIRST);
            }

            if ( error == MMP_ERR_NONE) {
                
                AHC_OSDDrawBuffer(m_ulDrawingThumbOSD,
                                  (UINT8*)TargetThumbAddr,
                                  AIHC_Thumb_CalculatePos( ThumbPosX, TargetThumbWidth, usTmpW ),
                                  AIHC_Thumb_CalculatePos( ThumbPosY, TargetThumbHeight, usTmpH ),
                                  usTmpW,
                                  usTmpH,
                                  OSDColor,ubLCDstatus);
            }
            else {
                if(m_FileDamage == NULL) {
                    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
                    AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GREEN);
                    #else
                    AHC_OSDSetColor(GUI_GREEN);
                    #endif
                    AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                        ThumbPosX,
                                        ThumbPosY,
                                        ThumbPosX + TargetThumbWidth,
                                        ThumbPosY + TargetThumbHeight);
                }
                else {
                    ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
                    ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;

                    AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY);
                }
            }
        }
    }
    else if ( ( ubFileType == DCF_OBG_MP3 ) ||
              ( ubFileType == DCF_OBG_WAV ) ||
              ( ubFileType == DCF_OBG_OGG ) ||
              ( ubFileType == DCF_OBG_WMA ) )
    {
        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_BLUE);
        #else
        AHC_OSDSetColor(GUI_BLUE);
        #endif

        AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                            ThumbPosX,
                            ThumbPosY,
                            ThumbPosX + TargetThumbWidth,
                            ThumbPosY + TargetThumbHeight);
    }
    else 
    {
        //TBD, Not supported file
        if(m_FileDamage == NULL) {
            #ifdef ENABLE_GUI_SUPPORT_MULTITASK
            AHC_OSDSetColor(m_ulDrawingThumbOSD, GUI_GREEN);
            #else
            AHC_OSDSetColor(GUI_GREEN);
            #endif
            AHC_OSDDrawFillRect(m_ulDrawingThumbOSD,
                                ThumbPosX,
                                ThumbPosY,
                                ThumbPosX + TargetThumbWidth,
                                ThumbPosY + TargetThumbHeight);
        }
        else {
            ThumbPosX += (TargetThumbWidth - m_FileDamage->XSize) >> 1;
            ThumbPosY += (TargetThumbHeight - m_FileDamage->YSize) >> 1;

            AHC_OSDDrawBitmap(m_ulDrawingThumbOSD, m_FileDamage, ThumbPosX, ThumbPosY + 1);
        }
    }

    //backup for double buffer display
    BEGIN_LAYER(m_ulDisplayThumbOSD);
    AHC_OSDDuplicateBuffer(m_ulDrawingThumbOSD, m_ulDisplayThumbOSD);
    END_LAYER(m_ulDisplayThumbOSD);
    
    //restore color
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(m_ulDrawingThumbOSD, CurrentGUIColor);
    AHC_OSDSetBkColor(m_ulDrawingThumbOSD, CurrentBkColor);
    #else
    AHC_OSDSetColor(CurrentGUIColor);
    AHC_OSDSetBkColor(CurrentBkColor);
    #endif
    
    AIHC_DrawReservedOSD(AHC_FALSE);
    END_LAYER(m_ulDrawingThumbOSD);
    
    return AHC_TRUE;
}

#if 0
void __BROWSER_API__(){}
#endif

/**
 @brief Config thumbnail mode

 This API configures the playback thumbnail mode.
 Parameters:
 @param[in] *pConfig The thumbnail configurations.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_Config(AHC_THUMB_CFG *pConfig, AHC_BOOL bKeephumbIndex)
{
    m_AHCThumb.usThumbPerRow           = pConfig->uwTHMPerLine;
    m_AHCThumb.usThumbPerCol           = pConfig->uwTHMPerColume;
    m_AHCThumb.usThumbHorizontalGap    = pConfig->uwHGap;
    m_AHCThumb.usThumbVerticalGap      = pConfig->uwVGap;
    m_AHCThumb.ulBkColor               = pConfig->byBkColor;
    m_AHCThumb.ulFocusColor            = pConfig->byFocusColor;
    m_AHCThumb.StartX                  = pConfig->StartX;
    m_AHCThumb.StartY                  = pConfig->StartY;
    m_AHCThumb.DisplayWidth            = pConfig->DisplayWidth;
    m_AHCThumb.DisplayHeight           = pConfig->DisplayHeight;

    if(!bKeephumbIndex) {
        m_AHCThumb.uwCurIdx        = 0;
    }
    
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetConfig(AHC_THUMB_CFG *pConfig)
{
    pConfig->uwTHMPerLine 	= m_AHCThumb.usThumbPerRow;        
    pConfig->uwTHMPerColume = m_AHCThumb.usThumbPerCol;
    pConfig->uwHGap			= m_AHCThumb.usThumbHorizontalGap;
    pConfig->uwVGap			= m_AHCThumb.usThumbVerticalGap;
    pConfig->byBkColor		= m_AHCThumb.ulBkColor;
    pConfig->byFocusColor	= m_AHCThumb.ulFocusColor;
    pConfig->StartX			= m_AHCThumb.StartX;
    pConfig->StartY			= m_AHCThumb.StartY;
    pConfig->DisplayWidth	= m_AHCThumb.DisplayWidth;
    pConfig->DisplayHeight	= m_AHCThumb.DisplayHeight;
    
    return AHC_TRUE;
}

/**
@brief	confirm the attribute of file is read only or not.

@param[in] wDcfObjIdx: the position of dcf object.

*/
AHC_BOOL AHC_Thumb_IsFileProtect(UINT32 wDcfObjIdx)
{
    AHC_BOOL bReadOnly = AHC_FALSE;
    
    AHC_UF_IsReadOnlybyIndex(wDcfObjIdx, &bReadOnly);
        
    if(bReadOnly == AHC_TRUE){
        return AHC_TRUE;
    
    }else{
        return AHC_FALSE;	
    }
}

AHC_BOOL AHC_Thumb_IsFileLocked(UINT32 wDcfObjIdx)
{
    AHC_BOOL bCharLock = AHC_FALSE;
    
    AHC_UF_IsCharLockbyIndex(wDcfObjIdx, &bCharLock); 

    if(bCharLock == AHC_TRUE){
        return AHC_TRUE;
    
    }else{
        return AHC_FALSE;
    }
}

AHC_BOOL AHC_Thumb_ProtectKey(void)
{
#ifdef CAR_DV
    UINT16   ThumbIndex;
    UINT16   TempDcfObjIdx;
    UINT16 	 ulFirstIndex;
    UINT16   ulObjCount;
    UINT16   i;
    AHC_BOOL bReadOnly = AHC_FALSE;
    AHC_BOOL bCharLock = AHC_FALSE;

    AHC_Thumb_GetFirstIdxCurPage(&ulFirstIndex);
    AHC_Thumb_GetCountCurPage(&ulObjCount);

    for( i = 0 ; i < ulObjCount ; i++){
    
        TempDcfObjIdx = ulFirstIndex + i;
    

        AHC_UF_IsCharLockbyIndex(TempDcfObjIdx, &bCharLock);
        AHC_UF_IsReadOnlybyIndex(TempDcfObjIdx, &bReadOnly);  
    

        if((bReadOnly == AHC_TRUE) || (bCharLock == AHC_TRUE))
        {
            ThumbIndex = m_AHCThumb.uiIdxMap[ (TempDcfObjIdx % (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol)) ];

            if (bReadOnly == AHC_TRUE)
            {
                AIHC_DrawProtectKey(ThumbIndex, AHC_PROTECT_MENU);
            }
            else if (bCharLock == AHC_TRUE)
            {
                AIHC_DrawProtectKey(ThumbIndex, AHC_PROTECT_G);
            }
        }
    }

    #ifdef FLM_GPIO_NUM
    AHC_OSDRefresh_PLCD();
    #endif
#endif

    return AHC_TRUE;
}


AHC_BOOL AHC_Thumb_SetCurrentIdx(UINT16 uiIndex)
{
    m_AHCThumb.uwCurIdx = uiIndex;
    AHC_UF_SetCurrentIndex((UINT32)uiIndex);
    
    return AHC_TRUE;

}

/**
 @brief Thumbnail shift up

 Shift the selected displayed thumbnail up.
 Parameters:
 @param[in] byNum The number of thumbnail to shift.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_Rewind(UINT8 byNum)
{    
    UINT32 uiIdx;
    UINT32 uiMaxIdx = 0;
    UINT32 uiCurPage = 0;
    UINT32 uiNextPage = 0;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);

    if( uiMaxIdx == 0 )
    {
        return AHC_FALSE;
    }

    uiMaxIdx -= 1;
    
    
    if(m_AHCThumb.uwCurIdx >= byNum)
        uiIdx = m_AHCThumb.uwCurIdx - byNum;
    else
        uiIdx = uiMaxIdx;//
    
    uiCurPage  = (m_AHCThumb.uwCurIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    uiNextPage = (uiIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    if ( uiCurPage ==  uiNextPage ) {
    
        if ( uiIdx != m_AHCThumb.uwCurIdx ) {
        
            AIHC_Thumb_DrawFocus(uiIdx, AHC_TRUE);
            m_AHCThumb.uwCurIdx = uiIdx;
            AHC_UF_SetCurrentIndex(uiIdx); // jeff.li
        }
    }
    else {
        m_AHCThumb.uwCurIdx = uiIdx;
        AHC_UF_SetCurrentIndex(uiIdx);
        
        AHC_Thumb_DrawPage(AHC_FALSE);
    }
    
    return AHC_TRUE;
}

/**
 @brief Thumbnail shift down

 Shift the selected displayed thumbnail down.
 Parameters:
 @param[in] byNum The number of thumbnail to shift.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_Forward(UINT8 byNum)
{    
    UINT32 uiIdx;
    UINT32 uiMaxIdx   = 0;
    UINT32 uiCurPage  = 0;
    UINT32 uiNextPage = 0;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        return AHC_FALSE;
    }

    uiMaxIdx -= 1;
    
    
    if ( m_AHCThumb.uwCurIdx + byNum > uiMaxIdx )
        uiIdx = 0;
    else
        uiIdx = m_AHCThumb.uwCurIdx + byNum;
    
    uiCurPage  = (m_AHCThumb.uwCurIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    uiNextPage = (uiIdx               ) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    if ( uiCurPage ==  uiNextPage ) {
        if ( uiIdx != m_AHCThumb.uwCurIdx ) {
            AIHC_Thumb_DrawFocus(uiIdx, AHC_TRUE);
            
            m_AHCThumb.uwCurIdx = uiIdx;
            AHC_UF_SetCurrentIndex(uiIdx); // jeff.li
        }
    }
    else {
        m_AHCThumb.uwCurIdx = uiIdx;
        AHC_UF_SetCurrentIndex(uiIdx);
        
        AHC_Thumb_DrawPage(AHC_FALSE);
    }
    
    return AHC_TRUE;
}

/**
 @brief Thumbnail Previous page

 Shift to previous page in Browser Mode.
 Parameters:
 @param[in] 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_RewindPage(void)
{    
    UINT32 uiIdx;
    UINT32 uiMaxIdx     = 0;
    UINT32 uiCurPage    = 0;
    UINT32 uiTotalPage  = 0;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    
    
    uiTotalPage  = uiMaxIdx / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    uiCurPage  = (m_AHCThumb.uwCurIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);

    if( uiCurPage > 0)
    {
        // rewind page
        uiCurPage--;// 0~n-1, n pages.
    }
    else
    {
        uiCurPage = uiTotalPage;
    }

    uiIdx = uiCurPage*(m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    
    m_AHCThumb.uwCurIdx = uiIdx; 
    AHC_UF_SetCurrentIndex(uiIdx);
        
    AHC_Thumb_DrawPage(AHC_FALSE);
    
    return AHC_TRUE;
}

/**
 @brief Thumbnail Previous page

 Shift to next page in Browser Mode.
 Parameters:
 @param[in] 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_ForwardPage(void)
{    
    UINT32 uiIdx;
    UINT32 uiMaxIdx 		= 0;
    UINT32 uiCurPage 		= 0;
    UINT32 uiTotalPage 	= 0;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    

    uiTotalPage  = uiMaxIdx / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    uiCurPage  = (m_AHCThumb.uwCurIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);

    if( uiTotalPage > uiCurPage)
    {
        uiCurPage++;
    }
    else
    {
        uiCurPage = 0;
    }

    uiIdx = uiCurPage*(m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    m_AHCThumb.uwCurIdx = uiIdx;
    AHC_UF_SetCurrentIndex(uiIdx);
        
    AHC_Thumb_DrawPage(AHC_FALSE);
    
    return AHC_TRUE;
}


AHC_BOOL AHC_Thumb_GetFirstIdx(UINT32 *puiIdx)
{
    UINT32 uiMaxIdx;
    
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        *puiIdx = 0;
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetPrevIdx(UINT32 *puiIdx)
{
    UINT32 uiMaxIdx;
    UINT32 uiIdx;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    AHC_UF_GetCurrentIndex(&uiIdx);
    
    if( uiMaxIdx == 0 )
    {
        *puiIdx = 0;
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    
    if(uiIdx > 0){
        *puiIdx = uiIdx - 1;
    }else{
        *puiIdx = uiMaxIdx;
    }
    
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetNextIdx(UINT32 *puiIdx)
{

    UINT32 uiMaxIdx;
    UINT32 uiIdx;
    
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    AHC_UF_GetCurrentIndex(&uiIdx);
    
    if( uiMaxIdx == 0 )
    {
        *puiIdx = 0;
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    
    if(uiIdx + 1 > uiMaxIdx){
        *puiIdx = 0;
    }else{
        *puiIdx = uiIdx + 1;
    }
    
    return AHC_TRUE;



}

AHC_BOOL AHC_Thumb_SetCurIdx(UINT32 uiIdx)
{
    AIHC_Thumb_SetCurrentIdx(uiIdx);
    AHC_UF_SetCurrentIndex(uiIdx);
    
    return AHC_TRUE;
}

AHC_BOOL AHC_Thumb_GetCurIdx(UINT32 *puiIdx)
{

    UINT32 uiMaxIdx;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        *puiIdx = 0;
        return AHC_FALSE;
    }
    
    *puiIdx = m_AHCThumb.uwCurIdx;
    
    return AHC_TRUE;


}

AHC_BOOL AHC_Thumb_GetSorting(void)
{
    #if (SORT_TYPE == SORT_TYPE_INCREASE)
    return AHC_TRUE;//increase
    #elif (SORT_TYPE == SORT_TYPE_DECREASE)
    return AHC_FALSE;//decrease
    #endif

}

AHC_BOOL AHC_Thumb_GetCurPos(UINT32 *puiPos)
{
    UINT32 uiMaxIdx;
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        *puiPos = 0;
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    
    *puiPos = m_AHCThumb.uwCurIdx;
    
    return AHC_TRUE;
}
/**
1~n
*/
AHC_BOOL AHC_Thumb_GetCurPage(UINT16 *puwPage)
{

    UINT32 uiMaxIdx;
    
    AHC_UF_GetTotalFileCount(&uiMaxIdx);
    
    if( uiMaxIdx == 0 )
    {
        *puwPage = 0;
        return AHC_FALSE;
    }
    
    uiMaxIdx -= 1;
    
    
    *puwPage = m_AHCThumb.uwCurIdx/(m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    
    *puwPage += 1;
    
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_Thumb_GetTotalPages
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_Thumb_GetTotalPages(UINT16 *puwPages)
{
    UINT32 uiCount;
    
    AHC_UF_GetTotalFileCount(&uiCount);
    
    if (uiCount == 0) {
        *puwPages = 0;
        return AHC_FALSE;
    }
    
    *puwPages = uiCount / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_Thumb_GetMaxNumPerPage
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_Thumb_GetMaxNumPerPage(UINT32  *ObjNumPerPage)
{
    *ObjNumPerPage = m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol;
   
    return AHC_TRUE;
}

/**
 @brief This API get the max DCF objects in current page.

 Parameters:
 @param[in] 
 @param[out] *ulObjCount   the DCF Obj Count in the current page.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_GetCountCurPage(UINT16 *ulObjCount)
{    
    UINT16 uiCount  = 0;
    UINT32 uiMaxIdx    = 0;
    UINT32 uiCurPage   = 0;
    UINT32 uiTotalPage = 0;

    AHC_UF_GetTotalFileCount(&uiMaxIdx);

    if( uiMaxIdx == 0 )
    {
        *ulObjCount = 0;
        return AHC_FALSE;
    }

    uiMaxIdx -= 1;

    uiTotalPage  = uiMaxIdx / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
#ifndef CAR_DV
    uiCurPage  = (uiMaxIdx - m_AHCThumb.uwCurIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);  
#else	  
    uiCurPage  = (m_AHCThumb.uwCurIdx) / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
#endif
    
    if( uiTotalPage > uiCurPage)
    {
        // 0~n-2 pages
        uiCount = m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol;
    
    }else{
        // n-1 page => the last page
        uiCount = uiMaxIdx - uiCurPage*(m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol) + 1;
    }
    
    *ulObjCount = uiCount;	
    
    return AHC_TRUE;
}

/**
 @brief This API get the current thumbnail index in current page.

 Parameters:
 @param[in] 
 @param[out] *ulObjCount   the current Obj index in the current page.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_GetCurPosCurPage(UINT16 *ulObjIndex)
{
    UINT32 uiIdx;
    UINT32 uiMaxIdx = 0;

    AHC_UF_GetTotalFileCount(&uiMaxIdx);

    if( uiMaxIdx == 0 )
    {
        return AHC_FALSE;
    }
    uiMaxIdx -= 1;
    
    AIHC_Thumb_GetCurrentIdx(&uiIdx);

    uiIdx = uiIdx%(m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);

    
    *ulObjIndex = uiIdx;
  
    return AHC_TRUE;
}

/**
 @brief Get first thumbnail index

 Retrieve the index of the fist displayed thumbnail in playback thumbnail.
 Parameters:
 @param[out] *pwIndex The position of first displayed thumbnail index.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_Thumb_GetFirstIdxCurPage(UINT16 *pwIndex)
{
    UINT32 temp;
    UINT32 uiMaxIdx;

    AHC_UF_GetTotalFileCount(&uiMaxIdx);

    if( uiMaxIdx == 0 )
    {
        return AHC_FALSE;
    }
    uiMaxIdx -= 1;
    
    temp = m_AHCThumb.uwCurIdx / (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    *pwIndex = temp * (m_AHCThumb.usThumbPerRow * m_AHCThumb.usThumbPerCol);
    
    
    return AHC_TRUE;
}

/**
 @brief Get audio file attribute

 This API gets the information of an video file. The file must be selected as 
 current DCF object first.
 @param[out] *pVideoAttr Address for placing the video attributes.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetVideoFileAttr(AHC_VIDEO_ATTR *pVideoAttr)
{
    UINT32                      CurrentDcfIdx,CurFileType;
    char                        FilePathName[MAX_FILE_NAME_SIZE];
    UINT8                       ubFileType;
    UINT32                      CurMode;
    MMPS_VIDEO_CONTAINER_INFO   VideoInfo;
    MMP_ERR                     err;
    
    if (!pVideoAttr) {
        return AHC_FALSE;
    }

    MEMSET(pVideoAttr, 0, sizeof(AHC_VIDEO_ATTR) );
    
    AHC_GetSystemStatus(&CurMode);
    
    CurMode >>= 16;
    
    if ( (AHC_MODE_IDLE == CurMode) || (AHC_MODE_THUMB_BROWSER == CurMode) ){
        AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
        
        MEMSET(FilePathName, 0, sizeof(FilePathName));
        AHC_UF_GetFilePathNamebyIndex(CurrentDcfIdx, FilePathName);
        AHC_UF_GetFileTypebyIndex(CurrentDcfIdx, &ubFileType); 

        if ( ( ubFileType == DCF_OBG_MOV ) ||
             ( ubFileType == DCF_OBG_MP4 ) ||
             ( ubFileType == DCF_OBG_AVI ) ||
             ( ubFileType == DCF_OBG_3GP ) ||
             ( ubFileType == DCF_OBG_WMV ) ) {
            err = MMPS_VIDPLAY_GetContainerInfo(FilePathName, STRLEN(FilePathName), &VideoInfo);
        }
        else
            return AHC_FALSE;
    }
    else if (AHC_MODE_PLAYBACK == CurMode) {
        AIHC_GetCurrentPBFileType(&CurFileType);
        
        if ( ( CurFileType == DCF_OBG_MOV ) ||
             ( CurFileType == DCF_OBG_MP4 ) ||
             ( CurFileType == DCF_OBG_AVI ) ||
             ( CurFileType == DCF_OBG_3GP ) ||
             ( CurFileType == DCF_OBG_WMV ) ) {
             
            err = MMPS_VIDPLAY_GetFileInfo(NULL, 0, &VideoInfo);
        }
        else
            return AHC_FALSE;
    }
    else {
        return AHC_FALSE;
    }
    
    if (MMP_ERR_NONE == err) {
        pVideoAttr->VideoAvailable[0]  = VideoInfo.is_video_available[0];
        pVideoAttr->VideoAvailable[1]  = VideoInfo.is_video_available[1];
        pVideoAttr->AudioAvailable  = VideoInfo.is_audio_available;
        pVideoAttr->Seekable[0]        = VideoInfo.is_video_seekable[0];        
        pVideoAttr->Seekable[1]        = VideoInfo.is_video_seekable[1];        
        pVideoAttr->TotalTime       = VideoInfo.total_file_time;
        
        pVideoAttr->VideoFormat[0]     = VideoInfo.video_info[0].format;
        pVideoAttr->VideoFormat[1]     = VideoInfo.video_info[1].format;
        pVideoAttr->Width[0]           = VideoInfo.video_info[0].width;
        pVideoAttr->Height[0]          = VideoInfo.video_info[0].height;
        pVideoAttr->Width[1]           = VideoInfo.video_info[1].width;
        pVideoAttr->Height[1]          = VideoInfo.video_info[1].height;
      
        pVideoAttr->AudioFormat     = VideoInfo.audio_info.format;
        pVideoAttr->SampleRate      = VideoInfo.audio_info.sampling_rate;
        pVideoAttr->Channels        = VideoInfo.audio_info.channel_nums; 
    }
    else {
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetVideoCurrentPlayTime
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get video current play time

 This API gets the current time of playing video file.
 @param[in] *pulTime The current time of playing audio file.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetVideoCurrentPlayTime(UINT32 *pulTime)
{
    MMP_ULONG ulCurTime;
    
    MMPS_VIDPLAY_GetCurrentTime(&ulCurTime);
    
    *pulTime = ulCurTime;
    
    return AHC_TRUE;
}

/**
 @brief Play clip

 This functions controls playback of video and audio files and it works 
 only under playback mode.
 Parameters:
 @param[in] bCommand Commands for controlling.
 @param[in] iOp Operand.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PlaybackClipCmd(AHC_PLAYBACK_CMDS bCommand, UINT32 iOp)
{
    UINT16          volume;
    UINT32          ret = AHC_TRUE;
    MMP_ERR         error;
    MMP_M_STATE     state;
    UINT16          DispWidth, DispHeight, Height;
    
    MMPS_VIDPLAY_SCALERCONFIG	cfg;
    
    switch(bCommand) {
    case AHC_PB_MOVIE_PAUSE:
        ret = MMPS_VIDPLAY_Pause();
        break;
    case AHC_PB_MOVIE_RESUME:
        ret = MMPS_VIDPLAY_Resume();
        break;
    case AHC_PB_MOVIE_FAST_FORWARD:
        if(m_ulMovPlayFFSpeed > 1)
            ret = MMPS_VIDPLAY_SetPlaySpeed(MMPS_VIDPLAY_PLAYSPEED_FAST_FORWARD, (m_ulMovPlayFFSpeed - 1)*100);
        break;
    case AHC_PB_MOVIE_FAST_FORWARD_RATE:
        m_ulMovPlayFFSpeed = (iOp / 1000);
        break;
    case AHC_PB_MOVIE_BACKWARD:
        if(m_ulMovPlayRewSpeed > 1)
            ret = MMPS_VIDPLAY_SetPlaySpeed(MMPS_VIDPLAY_PLAYSPEED_FAST_BACKWARD, (m_ulMovPlayRewSpeed - 1)*100);
        break;
    case AHC_PB_MOVIE_NORMAL_PLAY_SPEED:    
        ret = MMPS_VIDPLAY_SetPlaySpeed(MMPS_VIDPLAY_PLAYSPEED_NORMAL, 100);
        break;
    case AHC_PB_MOVIE_BACKWARD_RATE:
        m_ulMovPlayRewSpeed = (iOp / 1000);
        break;
    case AHC_PB_MOVIE_AUDIO_MUTE:
        if(iOp) {
            MMPS_AUDIO_GetPlayVolume(&m_CurrentVolume);
            ret = MMPS_AUDIO_SetPlayVolume(0,0);
        }
        else {
            MMPS_AUDIO_GetPlayVolume(&volume);
            
            if(volume == 0) {
                ret = MMPS_AUDIO_SetPlayVolume(m_CurrentVolume,0);
            }
        }
        break;
    case AHC_PB_MOVIE_SEEK_BY_TIME:
        ret = MMPS_VIDPLAY_SeekByTime(iOp, MMPS_VIDPSR_NEXT_KEYFRAME);
        break;
    case AHC_PB_MOVIE_SEEK_TO_SOS:
        ret = MMPS_VIDPLAY_SeekByTime(0, MMPS_VIDPSR_SEEK_PRECISE);
        break;    
    case AHC_PB_MOVIE_ROTATE:
        error = MMPS_VIDPLAY_GetState(&state);
    
        if (MMP_M_STATE_EXECUTING == state) {
            MMPS_VIDPLAY_SetVideoDecodeEnable(MMP_FALSE);
        }

        AIHC_GetCurrentPBHeight(&Height);
        AHC_Display_GetWidthHdight(&DispWidth, &DispHeight);

		MMPS_VIDPLAY_SetScalerCfg(&cfg, cfg.bUseScaler, DispWidth, DispHeight, 0, 0);
        if ((Height == 1088) && (1080 == cfg.ulOutHeight)) {
            // 1080P@60 VR v.s. 1080P output => no scaling
            cfg.ulOutWidth  = 0;
            cfg.ulOutHeight = 0;
            cfg.bUseScaler  = MMP_FALSE;
        }
        else if (cfg.ulOutHeight < Height) {
            cfg.bUseScaler = MMP_TRUE;
            
            if (cfg.ulOutHeight > AHC_HD_VIDPLAY_MAX_HEIGHT) {
                cfg.ulOutWidth = AHC_HD_VIDPLAY_MAX_WIDTH;
                cfg.ulOutHeight = AHC_HD_VIDPLAY_MAX_HEIGHT;
            }
        }
        else {
            cfg.ulOutWidth = 0;
            cfg.ulOutHeight = 0;
            cfg.bUseScaler = MMP_FALSE;
        }
        
        if (MMP_M_STATE_EXECUTING == state) {
            ret = MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, iOp, MMP_FALSE, NULL, &cfg);
        }
        else {
            ret = MMPS_VIDPLAY_SetDisplayMode(VIDEOPLAY_MODE_MAX, iOp, MMP_TRUE, NULL, &cfg);
        }
        
        if (MMP_M_STATE_EXECUTING == state) {
            
            MMPS_VIDPLAY_SetVideoDecodeEnable(MMP_TRUE);
        }
    
        break;
    }
    
    if ( ret )
        return AHC_FALSE;
    else
        return AHC_TRUE;
}

void AHC_SetPlaySpeed(AHC_PLAYBACK_CMDS cmd, UINT32 speed)
{
    if (cmd == AHC_PB_MOVIE_FAST_FORWARD_RATE) {
        m_ulMovPlayFFSpeed = speed;
    }
    else if (cmd == AHC_PB_MOVIE_BACKWARD_RATE) {
        m_ulMovPlayRewSpeed = speed;
    }
}

/**
 @brief 

 Encode the first frame of video to JPEG.
 
 Parameters:
 @param[in] Input memory address for getting JPEG.
 @param[in] Size of allocated memory.
 @param[in] String of full-filepath with null terminal.
 @param[in] Expected width of JPEG.
 @param[in] Expected heigh of JPEG.
 @retval 0 if Success,
        -1	something wrong in lower layer
        -2	wrong parameter or memory size is not enough
*/
int AHC_Thumb_GetJPEGFromVidFF(	UINT8	*pSrcBufAddr,
                                UINT32	dwBufSize,
                                char 	*pszFullPath,
                                UINT16 	wWidth,
                                UINT16	wHeight)
{
    MMP_ULONG 	dwOutputSize;
    MMP_ERR		error = MMP_ERR_NONE;
    
    if (!pSrcBufAddr || !pszFullPath) {
        return -2;
    }
        
    dwOutputSize = dwBufSize;
    
    error = MMPS_VIDPLAY_GetFrameToJpeg(pszFullPath,
                                        strlen(pszFullPath),
                                        wWidth,
                                        wHeight,
                                        MMPS_3GP_GETFRAME_FIRST,
                                        (MMP_ULONG)pSrcBufAddr,
                                        &dwOutputSize);
    if (dwOutputSize > dwBufSize)
        return -2;
    if (error != MMP_ERR_NONE)
        return -1;
    
    return dwOutputSize;
}



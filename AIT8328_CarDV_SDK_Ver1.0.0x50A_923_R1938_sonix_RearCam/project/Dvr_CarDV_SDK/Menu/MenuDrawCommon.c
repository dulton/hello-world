/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_GUI.h"
#include "AHC_Display.h"
#include "AHC_Utility.h"
#include "AHC_General_CarDV.h"
#include "MenuCommon.h"
#include "MenuDrawCommon.h"
#include "MenuDrawingFunc.h"
#include "ColorDefine.h"
#include "StateTVFunc.h"
#include "StateHdmiFunc.h"
#include "disp_drv.h"

/*===========================================================================
 * Global variable
 *===========================================================================*/ 

static UINT8  mLay1AvailableBuffer 	= 0;
static UINT8  mLay1CurrentBuffer 	= 0;
static UINT8  mLay1LastBuffer    	= 0 ;
static UINT8  mLay0AvailableBuffer 	= 0;
static UINT8  mLay0CurrentBuffer 	= 0;
static UINT8  mLay0LastBuffer    	= 0 ;
static INT8   m_byEnterDrawing      = 0;
/*===========================================================================
 * Extern variable
 *===========================================================================*/ 

extern AHC_BOOL bPreviewModeTVout;
extern AHC_BOOL gbAhcDbgBrk;
extern AHC_OS_SEMID gulDrawingSemID;
extern UINT16 TVFunc_GetUImodeOsdID(void);
extern UINT16 TVFunc_GetMenuOsdID(void);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
void _____MainLayer_Function_________(){ruturn;} //dummy
#endif

void OSDDraw_GetMainDrawBuffer(UINT8* ulID)
{
    if(0 == mLay0LastBuffer)
    {
        mLay0AvailableBuffer = MAIN_BUFFER_INDEX;
		mLay0LastBuffer      = MAIN_BUFFER_INDEX1;
    }
    else if (MAIN_BUFFER_INDEX == mLay0LastBuffer)
    {
        mLay0AvailableBuffer = MAIN_BUFFER_INDEX1;
    }
    else
    {
        mLay0AvailableBuffer = MAIN_BUFFER_INDEX;
    }
    *ulID = mLay0AvailableBuffer;
}

void OSDDraw_GetLastMainDrawBuffer(UINT8* ulID)
{
    *ulID = mLay0LastBuffer;
}

void OSDDraw_SetMainDrawBuffer(UINT8 ulID)
{
    mLay0CurrentBuffer = ulID;
    mLay0LastBuffer = mLay0CurrentBuffer;
}

void OSDDraw_GetDisplaySize(UINT8 bID, UINT16 *pw, UINT16* ph)
{
    UINT16 format;
    UINT32 addr;

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    AHC_DISPLAY_OUTPUTPANEL OutputDevice;

    AHC_GetDisplayOutputDev(&OutputDevice);

    if ((OutputDevice == AHC_DISPLAY_MAIN_LCD) || (OutputDevice == AHC_DISPLAY_SUB_LCD))
    {
        AHC_OSDGetBufferAttr(bID, ph, pw, &format, &addr);
    }
    else
#endif
    {
        AHC_OSDGetBufferAttr(bID, pw, ph, &format, &addr);
    }
}

void OSDDraw_ClearMainDrawBuffer(void)
{
    UINT8 bID = 0;
    OSDDraw_GetLastMainDrawBuffer(&bID);
    OSDDraw_ClearPanel(bID, OSD_COLOR_TRANSPARENT);
}

void OSDDraw_ClearMainDrawBufferAll(void)
{
    OSDDraw_ClearPanel(MAIN_DISPLAY_BUFFER, OSD_COLOR_TRANSPARENT);

    AHC_OSDDuplicateBuffer(MAIN_DISPLAY_BUFFER, MAIN_BUFFER_INDEX);
    AHC_OSDDuplicateBuffer(MAIN_DISPLAY_BUFFER, MAIN_BUFFER_INDEX1);
}

#if 0
void _____MenuLayer_Function_________(){ruturn;} //dummy
#endif

void OSDDraw_GetMenuDrawBuffer(UINT8* ulID)
{
#if HDMI_ENABLE
	if(HDMIFunc_IsInHdmiMode())
	{
        if(HDMI_MENU_OSD_ID != mLay1LastBuffer &&
           HDMI_MENU_OSD2_ID != mLay1LastBuffer)
        {
            mLay1AvailableBuffer = HDMI_MENU_OSD_ID;
    		mLay1LastBuffer      = HDMI_MENU_OSD2_ID;
        }
        else if (HDMI_MENU_OSD2_ID == mLay1LastBuffer)
        {
            mLay1AvailableBuffer = HDMI_MENU_OSD_ID;
        }
        else
        {
            mLay1AvailableBuffer = HDMI_MENU_OSD2_ID;
        }
	}	
	else
#endif
#if TVOUT_ENABLE
	if(TVFunc_IsInTVMode())
	{
        if(TV_MENU_OSD_ID != mLay1LastBuffer &&
           TV_MENU_OSD2_ID != mLay1LastBuffer)
        {
            mLay1AvailableBuffer = TV_MENU_OSD_ID;
    		mLay1LastBuffer      = TV_MENU_OSD2_ID;
        }
        else if (TV_MENU_OSD2_ID == mLay1LastBuffer)
        {
            mLay1AvailableBuffer = TV_MENU_OSD_ID;
        }
        else
        {
            mLay1AvailableBuffer = TV_MENU_OSD2_ID;
        }
	}
	else
#endif
    {
        if(OVL_BUFFER_INDEX2 != mLay1LastBuffer &&
           OVL_BUFFER_INDEX3 != mLay1LastBuffer)
        {
            mLay1AvailableBuffer = OVL_BUFFER_INDEX2;
    		mLay1LastBuffer      = OVL_BUFFER_INDEX3;
        }
        else if (OVL_BUFFER_INDEX2 == mLay1LastBuffer)
        {
            mLay1AvailableBuffer = OVL_BUFFER_INDEX3;
        }
        else
        {
            mLay1AvailableBuffer = OVL_BUFFER_INDEX2;
        }
    }

    *ulID = mLay1AvailableBuffer;
}

void OSDDraw_GetLastMenuDrawBuffer(UINT8* ulID)
{
    *ulID = mLay1LastBuffer;
}

void OSDDraw_SetMenuDrawBuffer(UINT8 ulID)
{
    mLay1CurrentBuffer = ulID;
    mLay1LastBuffer = mLay1CurrentBuffer;
}

void OSDDraw_EnterMenuDrawing(UINT8* pbID0, UINT8* pbID1)
{
	OSDDraw_GetMenuDrawBuffer(pbID0);

	OSDDraw_GetLastMenuDrawBuffer(pbID1);
}

void OSDDraw_ExitMenuDrawing(UINT8* pbID0, UINT8* pbID1)
{
	OSDDraw_SetMenuDrawBuffer(*pbID0);

	AHC_OSDSetCurrentDisplay(*pbID0);

	AHC_OSDRefresh();
	
	AHC_OSDDuplicateBuffer(*pbID0, *pbID1);
} 

#if 0
void _____OverlayLayer_Function_________(){ruturn;} //dummy
#endif

void OSDDraw_GetOvlDrawBuffer(UINT8* ulID)
{
#if HDMI_SINGLE_OSD_BUF
	if(HDMIFunc_IsInHdmiMode())
	{
		*ulID = HDMIFunc_GetUImodeOsdID();
	}	
	else
#endif
#if TV_SINGLE_OSD_BUF
	if(TVFunc_IsInTVMode())
	{
		*ulID = TVFunc_GetUImodeOsdID();
	}
	else
#endif
    {
        if(OVL_BUFFER_INDEX  == mLay1LastBuffer)
        {
            mLay1AvailableBuffer = OVL_BUFFER_INDEX1;
        }
        else if (OVL_BUFFER_INDEX1 == mLay1LastBuffer)
        {
            mLay1AvailableBuffer = OVL_BUFFER_INDEX;
        }
        else
        {
            mLay1AvailableBuffer = OVL_BUFFER_INDEX;
            mLay1LastBuffer      = OVL_BUFFER_INDEX1;
        }

        *ulID = mLay1AvailableBuffer;
    }
}

void OSDDraw_GetLastOvlDrawBuffer(UINT8* ulID)
{
#if HDMI_SINGLE_OSD_BUF
	if(HDMIFunc_IsInHdmiMode())
	{
		*ulID = HDMIFunc_GetUImodeOsdID();
	}	
	else
#endif
#if TV_SINGLE_OSD_BUF
	if(TVFunc_IsInTVMode())
	{
		*ulID = TVFunc_GetUImodeOsdID();
	}
	else
#endif
    {
        *ulID = mLay1LastBuffer;
    }
}

void OSDDraw_SetOvlDrawBuffer(UINT8 ulID)
{
#if HDMI_SINGLE_OSD_BUF
	if(HDMIFunc_IsInHdmiMode())
	{
		mLay1CurrentBuffer = HDMIFunc_GetUImodeOsdID();
		mLay1LastBuffer = HDMIFunc_GetUImodeOsdID();
	}	
	else
#endif
#if TV_SINGLE_OSD_BUF
	if(TVFunc_IsInTVMode())
	{
		mLay1CurrentBuffer = TVFunc_GetUImodeOsdID();
		mLay1LastBuffer = TVFunc_GetUImodeOsdID();
	}
	else
#endif
    {
        mLay1CurrentBuffer = ulID;
        mLay1LastBuffer = mLay1CurrentBuffer;
    }
}

void OSDDraw_ClearOvlDrawBuffer(UINT8 ulID)
{
    OSDDraw_ClearPanel(ulID, OSD_COLOR_TRANSPARENT);
}

void OSDDraw_ClearOvlDrawOvlBufferAll(void)
{
#if HDMI_ENABLE
	if(HDMIFunc_IsInHdmiMode())
	{
        OSDDraw_ClearPanel(HDMIFunc_GetUImodeOsdID(), OSD_COLOR_TRANSPARENT);
	}	
	else
#endif
#if TVOUT_ENABLE
	if(TVFunc_IsInTVMode())
	{
        OSDDraw_ClearPanel(TVFunc_GetUImodeOsdID(), OSD_COLOR_TRANSPARENT);
	}
	else
#endif
    {
        OSDDraw_ClearPanel(OVL_DISPLAY_BUFFER, OSD_COLOR_TRANSPARENT);
        AHC_OSDDuplicateBuffer(OVL_DISPLAY_BUFFER, OVL_BUFFER_INDEX );
        AHC_OSDDuplicateBuffer(OVL_DISPLAY_BUFFER, OVL_BUFFER_INDEX1);
    }
}

void OSDDraw_EnterDrawing(UINT8* pbID0, UINT8* pbID1)
{
	if (AHC_OS_AcquireSem(gulDrawingSemID, 0) != OS_NO_ERR) {
		printc("ASSERT:%s  gulDrawingSemID LOCKED\r\n", __func__);
	}

    //OSDDraw_EnterDrawing monitor functions, only one function can call it in the meantime
    m_byEnterDrawing++;
    if(m_byEnterDrawing != 1)
    {
        printc(FG_YELLOW("m_byEnterDrawing %d\r\n"),m_byEnterDrawing);
        printc(FG_YELLOW("====Warning!!! Illegal access function %s====\r\n"),__func__);
    }
	//
#if HDMI_SINGLE_OSD_BUF
	if(HDMIFunc_IsInHdmiMode())
	{
		*pbID0 = HDMIFunc_GetUImodeOsdID();
		*pbID1 = HDMIFunc_GetUImodeOsdID();
	}	
	else
#endif
#if TV_SINGLE_OSD_BUF
	if(TVFunc_IsInTVMode())
	{
		*pbID0 = TVFunc_GetUImodeOsdID();
		*pbID1 = TVFunc_GetUImodeOsdID();
	}
	else
#endif
	{
   		OSDDraw_GetOvlDrawBuffer(pbID0);
    	OSDDraw_GetLastOvlDrawBuffer(pbID1);
	}
}

void OSDDraw_ExitDrawing(UINT8* pbID0, UINT8* pbID1)
{
    OSDDraw_SetOvlDrawBuffer(*pbID0);
    AHC_OSDSetCurrentDisplay(*pbID0);
    AHC_OSDRefresh();

#if (HDMI_ENABLE) || (TVOUT_ENABLE)
	if( !TVFunc_IsInTVMode() || !HDMIFunc_IsInHdmiMode() )
#endif
	{
	    AHC_OSDDuplicateBuffer(*pbID0, *pbID1);
	}

	//OSDDraw_ExitDrawing monitor functions, only one function can call it in the meantime
	m_byEnterDrawing--;
	if(m_byEnterDrawing != 0)
	{
		printc(FG_YELLOW("m_byEnterDrawing %d\r\n"),m_byEnterDrawing);
		printc(FG_YELLOW("====Warning!!! Illegal access function %s====\r\n"),__func__);
	}

    if (AHC_OS_ReleaseSem(gulDrawingSemID) != OS_NO_ERR) {
        printc("ASSERT:%s gulDrawingSemID UNLOCKED\r\n", __func__);
    }
}
 
#if 0
void _____Other_Function_________(){ruturn;} //dummy
#endif

void OSDDraw_ClearPanel(UINT16 ubID, GUI_COLOR color)
{
    UINT16 w, h;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return; }

#if (defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(CFG_DRAW_WIFI_STREAMING_MONO_PATTERN))
    if (AHC_Get_WiFi_Streaming_Status())
        color = CFG_DRAW_WIFI_STREAMING_MONO_PATTERN;
#endif

    BEGIN_LAYER(ubID);
    OSDDraw_GetDisplaySize(ubID, &w, &h);
#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(ubID, color);
#else
    AHC_OSDSetColor(color);
#endif
    AHC_OSDDrawFillRect(ubID, 0, 0, w, h);
    END_LAYER(ubID);
}

AHC_BOOL OSDDraw_SetAlphaBlending(UINT8 ulID, AHC_BOOL bEnable)
{
    UINT32 	iVal[10];
    UINT8 	i;
    
    iVal[0] = ( AHC_FALSE == bEnable )?(0):(1);
    iVal[1] = AHC_OSD_ALPHA_ARGB;

    for(i=0; i<8; i++) 
    {
        iVal[2+i] = i * 256 / 8;
    }

    return AHC_OSDSetDisplayAttr( ulID, AHC_OSD_ATTR_ALPHA_BLENDING_ENABLE, iVal );
}   

AHC_BOOL OSDDraw_SetSemiTransparent(UINT8 ulID, UINT8 ulEnable, UINT8 ulOsdSemiType, UINT8 ulSemiWeight )
{
    UINT32 iVal[10];

    iVal[0] = (UINT32)ulEnable;
    iVal[1] = (UINT32)ulOsdSemiType;   //  AHC_OSD_SEMITP_AVG
                                       //  AHC_OSD_SEMITP_AND
                                       //  AHC_OSD_SEMITP_OR 
                                       //  AHC_OSD_SEMITP_INV

    iVal[2] = (UINT32) ulSemiWeight;   //  transparent weight 0~255

    return AHC_OSDSetDisplayAttr( ulID, AHC_OSD_ATTR_SEMI_TRANSPARENT_ENABLE, iVal );
}

/** @brief The general function to enable or disable the semi-transparent function.
 *
 * It uses the semi transparent feature based on the color depth of the buffer.
 *
 * @param id AHC buffer ID
 * @param enable enable or disable the buffer
 * @return The result from the semi-transparent function
 */
AHC_BOOL OSDDraw_EnableSemiTransparent(UINT8 id, AHC_BOOL enable)
{
    UINT16 format;
    UINT16 dummy;
    UINT32 addr;

    AHC_OSDGetBufferAttr(id, &dummy, &dummy, &format, &addr);
    if (format < OSD_COLOR_ALPHA_START) {//RGB without alpha
        return OSDDraw_SetSemiTransparent(id, enable, AHC_OSD_SEMITP_AVG, 170);//~0.66 alpha
    }
    return OSDDraw_SetAlphaBlending(id, enable);
}

void OSD_Draw_Icon(const GUI_BITMAP Bmp, RECT Draw, UINT16 ulID)
{   
    AHC_OSDDrawBitmap(ulID, &Bmp, Draw.uiLeft, Draw.uiTop);
}

void OSD_Draw_IconXY(UINT16 ulID, const GUI_BITMAP Bmp, UINT16 x, UINT16 y)
{   
    AHC_OSDDrawBitmap(ulID, &Bmp, x, y);
}

UINT8 NumOfDigit(UINT32 num)
{
    UINT8 k = 1;
    
	while((num/=10)>=1) k++;
    return k;
}

UINT8* GetFileNameAddr(char* filepath)
{
    UINT16 len = 0;
    char* addr;

    addr = filepath;
    while(*addr) {
    	len++;
    	addr++;
    }
    
    addr--;
    
	while((*addr != '\\') && (*addr != '/') && len) {
		len--;
		addr--;
	}
	
	if ((*addr == '\\') || (*addr == '/'))
	    addr++;

    return (UINT8*)addr;
}

static GUI_RECT m_BoundingRC;
/** @brief Reset the bounding rectangle structure.
 *
 *  @note The RC
 *
 *  @post Use OSDDraw_ScanBoundingRC or OSDDraw_ScanBoundingRCFromButton to scan the bounding rectangle.
 *        Typically for clearing the minimal rectangle to speed up menu drawing function.
 */
void OSDDraw_ResetBoundingRC(void)
{
    UINT16 format;
    UINT16 w, h;
    UINT32 addr;
    AHC_OSDGetBufferAttr(OVL_BUFFER_INDEX, &w, &h, &format, &addr);
    m_BoundingRC.x0 = w;
    m_BoundingRC.y0 = h;
    m_BoundingRC.x1 = 0;
    m_BoundingRC.y1 = 0;
}

GUI_RECT* OSDDraw_ScanBoundingRc(const RECT* rc)
{
    //printc("rc: %d, %d, (%dx%d)\r\n", rc->uiLeft, rc->uiTop, rc->uiWidth, rc->uiHeight);
    if (rc->uiLeft < m_BoundingRC.x0) {
        m_BoundingRC.x0 = rc->uiLeft;
    }

    if (rc->uiTop < m_BoundingRC.y0) {
        m_BoundingRC.y0 = rc->uiTop;
    }

    if (rc->uiLeft + rc->uiWidth > m_BoundingRC.x1) {
        m_BoundingRC.x1 = rc->uiLeft + rc->uiWidth;
    }

    if (rc->uiTop + rc->uiHeight > m_BoundingRC.y1) {
        m_BoundingRC.y1 = rc->uiTop + rc->uiHeight;
    }

    return &m_BoundingRC;
}

GUI_RECT* OSDDraw_ScanTouchBoundingRc(const SMENUTOUCHBUTTON* touch, UINT16 num)
{
    UINT16 i;

    for (i = 0; i < num; i++) {
        OSDDraw_ScanBoundingRc(&(touch[i].rcButton));
    }
    return &m_BoundingRC;
}

void OSDDraw_ClearTouchButtons(UINT16 uwDisplayID, SMENUTOUCHBUTTON* touchButtons, UINT16 num)
{
    OSDDraw_ResetBoundingRC();
    OSDDraw_ScanTouchBoundingRc(touchButtons, num);
	#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(uwDisplayID, OSD_COLOR_TRANSPARENT);
    #else
    AHC_OSDSetColor(OSD_COLOR_TRANSPARENT);
    #endif
    //printc("\r\n(%d,%d)~(%dx%d) has been scanned\r\n", m_BoundingRC.x0, m_BoundingRC.y0, m_BoundingRC.x1, m_BoundingRC.y1);
    AHC_OSDDrawFillRect(uwDisplayID, m_BoundingRC.x0, m_BoundingRC.y0, m_BoundingRC.x1, m_BoundingRC.y1);
}

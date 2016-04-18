
#ifndef _MENU_STATE_MODE_SELECT_MENU_H_
#define _MENU_STATE_MODE_SELECT_MENU_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_Gui.h"

/*===========================================================================
 * Enum define 
 *===========================================================================*/ 

#if defined(CAR_DV)

#if (USB_MODE_SELECT_EN)
typedef enum {

	ITEMID_USB_MSDC_MODE = 1,
	ITEMID_USB_PCAM_MODE,
	ITEMID_USB_DSC_MODE,
	ITEMID_USB_DV_MODE
}USBMODEITEMID;

typedef enum {

	USB_MSDC_MODE = 1,
	USB_PCAM_MODE,
	USB_DSC_MODE,
	USB_DV_MODE,
	USB_MAX_MODE
}USBMODE;

#endif

#if (UI_MODE_SELECT_EN)
typedef enum {

	ITEMID_UI_DSC_MODE = 1,
	ITEMID_UI_DV_MODE,
    ITEMID_UI_STILLB_MODE,
    ITEMID_UI_STILLP_MODE,
    ITEMID_UI_VIDEOB_MODE,
    ITEMID_UI_VIDEOP_MODE,
    ITEMID_UI_MENU_MODE,
    ITEMID_UI_BROWSER_MODE
}UIMODEITEMID;

typedef enum {
	
	UI_DSC_MODE = 1,
	UI_DV_MODE,
    UI_STILLB_MODE,
    UI_STILLP_MODE,
    UI_VIDEOB_MODE,
    UI_VIDEOP_MODE,
    UI_ALLB_MODE,
    UI_ALLP_MODE,
    UI_MENU_MODE,
    UI_BROW_SELECT_MODE,
    UI_MAX_MODE
}UIMODE;

#endif

#if (BROWSER_MODE_SELECT_EN)
typedef enum {

	ITEMID_BROWSER_EVENT = 1,
	ITEMID_BROWSER_VIDEO,
    ITEMID_BROWSER_PHOTO
}BROWSERMODEITEMID;

typedef enum {
	
	BROWSER_EVENT_MODE = 1,
	BROWSER_VIDEO_MODE,
    BROWSER_PHOTO_MODE,
    BROWSER_ALL_MODE,
    BROWSER_MAX_MODE
}BROWSERMODE;

#endif

#else//CAR_DV

typedef enum {	
    ITEMID_STILL_MODE= 1,

    ITEMID_STILLB_MODE,
    ITEMID_STILLP_MODE,
    ITEMID_VIDEOB_MODE,
    ITEMID_VIDEOP_MODE
}MOVIEITEMID;
#endif

/*===========================================================================
 * Function prototype 
 *===========================================================================*/ 

void MenuStateModeSelectMode( UINT32 ulEvent, UINT32 ulPosition );

#endif //_MENU_STATE_MODE_SELECT_MENU_H_



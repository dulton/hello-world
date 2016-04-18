//==============================================================================
//
//  File        : StateTVFunc.h
//  Description : INCLUDE File for the StateTVFunc function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _STATETVFUNC_H_
#define _STATETVFUNC_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "MenuCommon.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define	TV_UI_OSD_ID	(17)
#define	TV_MENU_OSD_ID	(18)
#define	TV_MENU_OSD2_ID	(19)

/*===========================================================================
 * Enum define
 *===========================================================================*/ 

typedef enum _AHC_TV_STATUS
{
    AHC_TV_NONE_STATUS = 0, 
    AHC_TV_BROWSER_STATUS,    
    AHC_TV_MOVIE_PB_STATUS,
    AHC_TV_PHOTO_PB_STATUS,
    AHC_TV_ALL_PB_STATUS,
    AHC_TV_SLIDE_SHOW_STATUS,
    AHC_TV_VIDEO_PREVIEW_STATUS,
    AHC_TV_DSC_PREVIEW_STATUS,
    AHC_TV_NET_PLAYBACK_STATUS,
    AHC_TV_SLEEP_STATUS
} AHC_TV_STATUS;

typedef enum _TV_DELETE_OP
{
	DEL_FILE_NONE = 0,
	DEL_FILE_SELECT_NO,
	DEL_FILE_SELECT_YES,
	DEL_FILE_DECIDE_NO,
	DEL_FILE_DECIDE_YES	
} TV_DELETE_OP;

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

AHC_BOOL 		TVFunc_IsConnect(void);
AHC_BOOL		TVFunc_IsInMenu(void);
void 			SetTVState(AHC_TV_STATUS TVState); 
void 			StateTVMode( UINT32 ulEvent );
void			StateTV_MoviePBMode_Handler(UINT32 ulJobEvent);
void			StateTV_PhotoPBMode_Handler(UINT32 ulJobEvent);
AHC_BOOL 		TVMode_Start(void);
AHC_TV_STATUS 	TVFunc_Status(void);

AHC_BOOL 		TVFunc_ThumbnailOption(UINT8 ubOp);
AHC_BOOL 		TVFunc_ThumbnailEditOneFile(void);
AHC_BOOL		TVFunc_ThumbnailModeSwitch(void);
AHC_BOOL 		TVFunc_PlaybackEditOneFile(UINT8 Op);
UINT16 			TVFunc_GetMenuOsdID(void);
UINT16 			TVFunc_GetUImodeOsdID(void);
UINT16 			TVFunc_InitPreviewOSD(void);
AHC_BOOL 		TVFunc_IsPreview(void);
AHC_BOOL 		TVFunc_IsPlayback(void);
AHC_BOOL 		TVFunc_IsInTVMode(void);
AHC_BOOL 		TVFunc_CheckTVStatus(UINT8 ubCheckStatus);

void			Scale_MenuOSD_Window(int xc);

#endif  // _STATETVFUNC_H_

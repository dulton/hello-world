/**
 @file AIHC_Display.h
 @brief Header File for the AIHC display API.
 @author 
 @version 1.0
*/

#ifndef _AIHC_DISPLAY_H_
#define _AIHC_DISPLAY_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Common.h"
#include "AHC_Display.h"
#include "mmps_display.h"

/*===========================================================================
 * Function prototype
 *===========================================================================*/

UINT16   AIHC_GetWinDupMode(void);
AHC_BOOL AIHC_Display_EnterLcd(void);
AHC_BOOL AIHC_Display_EnterHdmi(void);
AHC_BOOL AIHC_Display_EnterNtscTv(void);
AHC_BOOL AIHC_Display_EnterPalTv(void);

#endif //_AIHC_DISPLAY_H_
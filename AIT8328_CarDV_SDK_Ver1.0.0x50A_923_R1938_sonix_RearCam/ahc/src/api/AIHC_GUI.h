//==============================================================================
//
//  File        : AIHC_GUI.h
//  Description : INCLUDE File for the AHC GUI function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AIHC_GUI_H_
#define _AIHC_GUI_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Common.h"
#include "AHC_Parameter.h"
#include "AHC_GUI.h"
#include "AHC_Config_SDK.h"

/*===========================================================================
 * Function prototype
 *===========================================================================*/

AHC_BOOL AIHC_OSDGetParam(AHC_PARAM_ID wParamID,UINT32 *Param);
AHC_BOOL AIHC_OSDDrawBuffer(UINT8 uwDisplayID, UINT8 *InBuffer, UINT16 uwStartX, UINT16 uwStartY, UINT16 uwWidth, UINT16 uwHeight,UINT16 uwBufWidth, AHC_OSD_COLOR_FMT uiColorFormat);
AHC_BOOL AIHC_DrawReservedOSD(AHC_BOOL bDrawReserved);
AHC_BOOL AIHC_GUI_GetOSDBufAddr(UINT16 uwDisplayID, UINT32 *Addr);

#endif

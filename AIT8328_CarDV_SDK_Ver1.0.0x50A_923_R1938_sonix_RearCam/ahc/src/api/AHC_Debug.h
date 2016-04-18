/**
 @file AHC_Debug.h
 @brief Header File for the AHC debug API.
 @author 
 @version 1.0
*/

#ifndef _AHC_DEBUG_H_
#define _AHC_DEBUG_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"

/** @addtogroup AHC_DEBUG
@{
*/
/// @}

/*===========================================================================
 * Function prototype
 *===========================================================================*/ 

AHC_BOOL AHC_GetAITVersion(UINT16 *pwHwVersion, UINT16 *pwFwVersion);
AHC_BOOL AHC_SetDbgBreak(UINT8 ubSetDbgBrkEn);
#endif //_AHC_DEBUG_H_
//==============================================================================
//
//  File        : AHC_MIMF.c
//  Description : INCLUDE File for the Middleware of Index Mapping Functions porting.
//  Author      : Jerry Li
//  Revision    : 1.0
//
//=============================================================================
#ifndef _AHC_MIMF_H_
#define _AHC_MIMF_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Common.h"
#include "AHC_Config_SDK.h"
#include "AHC_General.h"
#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
#include "AHC_DCF.h"
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
#include "AHC_DCFDT.h"
#endif
/*===========================================================================
 * Enum define
 *===========================================================================*/
typedef enum _AHC_MIMF_BROWSER_MASK{
    AHC_MIMF_BROWSER_MASK_DB0       = (0x01 << 0),
    AHC_MIMF_BROWSER_MASK_DB1       = (0x01 << 1),
    AHC_MIMF_BROWSER_MASK_DB2       = (0x01 << 2),
    AHC_MIMF_BROWSER_MASK_DB3       = (0x01 << 3),
    AHC_MIMF_BROWSER_MASK_REAR_CAM  = (0x01 << 4),
    AHC_MIMF_BROWSER_MASK_ALL       = 0x1F
}AHC_MIMF_BROWSER_MASK;
//DCF_DB_TYPE

typedef enum _AHC_MIMF_BROWSER_MASK_TYPE{
    AHC_MIMF_BROWSER_MASK_MOV       = (0x01 << 0),
    AHC_MIMF_BROWSER_MASK_JPG       = (0x01 << 1),
    AHC_MIMF_BROWSER_MASK_TYPE_ALL  = 0x3
}AHC_MIMF_BROWSER_MASK_TYPE;

/*===========================================================================
 * Function prototype
 *===========================================================================*/
void     AHC_MIMF_SetBrowserMask(UINT32 uiBrowserMask);
void     AHC_MIMF_GetBrowserMask(UINT32 *puiBrowserMask);
AHC_BOOL AHC_MIMF_GetTotalFileCount(UINT32 *puiImgIndex);
AHC_BOOL AHC_MIMF_IndexReverseMapping(UINT32 uiImgIndex, UINT32 *puiDcfIndex, UINT32 *sDB, AHC_BOOL *bIsRearCam);
AHC_BOOL AHC_MIMF_IndexMapping(UINT32 puiDcfIndex, UINT32 sDB, AHC_BOOL bIsRearCam, UINT32 *uiImgIndex);
AHC_BOOL AHC_MIMF_SetCurrentIndex(UINT32 uiImgIndex);
AHC_BOOL AHC_MIMF_GetCurrentIndex(UINT32 *puiImgIndex);
AHC_BOOL AHC_MIMF_GetFilePathNamebyIndex(UINT32 uiIndex, char* FilePathName);
AHC_BOOL AHC_MIMF_GetFileSizebyIndex(UINT32 uiIndex, UINT32* uiFileSize);
AHC_BOOL AHC_MIMF_GetFileTypebyIndex(UINT32 uiIndex, UINT8 *ubFileType);
AHC_BOOL AHC_MIMF_IsReadOnlybyIndex(UINT32 uiIndex, AHC_BOOL *bReadOnly);
AHC_BOOL AHC_MIMF_GetFileTimebyIndex(UINT32 uiIndex, AHC_RTC_TIME *pRtcTime);
AHC_BOOL AHC_MIMF_IsCharLockbyIndex(UINT32 uiIndex, AHC_BOOL *bCharLock);
AHC_BOOL AHC_MIMF_GetFileNamebyIndex(UINT32 uiIndex, char* FileName);
void     AHC_MIMF_RegisterCusCallback(void);


#endif //end #ifndef _AHC_MIMF_H_


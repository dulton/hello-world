/** @file ait_utility.h

@brief Header file of the utility
@author Truman Yang
@since 10-Jul-06
@version
- 1.0 Original version
*/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "mmps_system.h"
#include "mmps_display.h"
#include "mmps_fs.h"
#include <string.h>
#include <stdlib.h>

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
/** @name API Portability
@{ */
    #define FS_USE_ICE  (0)
    #define OMA_DRM_EN  (0)

    #if defined(ALL_FW)
    extern void printc(char *szFormat, ...);
    #define FW_DBG_EN   (0)
        #if (FW_DBG_EN)
        #define PRINTF(...) {printc(__VA_ARGS__);}
        #else
        #define PRINTF(...) {do {} while(0);}
        #endif
    #else
    #define PRINTF(...) {do {} while(0);}
    #endif
    #define SCANF(...) {do {} while(0);}

    /** @brief Implement some standard functions and retarget to it. */
    #define STRLEN strlen
    #define STRCPY strcpy
    #define STRCMP strcmp
    #define STRCAT strcat
    #ifndef MEMCPY
    #define MEMCPY memcpy
    #endif
    #ifndef MEMSET
    #define MEMSET memset
    #endif
    #define MEMCMP memcmp
    #define MEMMOVE memmove
    #ifndef MCP_MMI_FLASH
    #define GETS(c) gets(c)
    #endif
    #define RAND() rand()

/** @} */ // end of API Portability

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef struct _MMP_TIME_STRUC
{
    MMP_USHORT uwYear;
    MMP_USHORT uwMonth;
    MMP_USHORT uwDay;
    MMP_USHORT uwHour;
    MMP_USHORT uwMinute;
    MMP_USHORT uwSecond;
} MMP_TIME_STRUC;

typedef MMP_BOOL AllocZeroMemCBFunc(MMP_LONG size, void **ptr);

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_DISPLAY_INIT_FUNC MMPC_Display_InitMainLCD;
MMP_DISPLAY_INIT_FUNC MMPC_Display_ReinitMainLCD;
MMP_BOOL MMPC_Display_InitLCD(void);

MMP_USHORT MMPC_FS_FileOpen(MMP_BYTE *bFileName, MMP_BYTE *bMode, void **FileHandle);
MMP_USHORT MMPC_FS_FileClose(void *FileHandle);
MMP_USHORT MMPC_FS_FileSeek(void *FileHandle, MMP_LONG64 file_offset, MMP_ULONG file_orign);
MMP_USHORT MMPC_FS_FileWrite(void *FileHandle, MMP_UBYTE *uData, MMP_ULONG ulNumBytes, MMP_ULONG *ulWriteCount);
MMP_USHORT MMPC_FS_FileRead(void *FileHandle, MMP_UBYTE *uData, MMP_ULONG ulNumBytes, MMP_ULONG *ulReadCount);
MMP_USHORT MMPC_FS_FileGetSize(void *FileHandle, MMP_ULONG64 *ulFileSize);
MMP_USHORT MMPC_FS_FileTell(void *FileHandle, MMP_LONG64 *llFilePos);

void MMPC_FS_BBGetRTC(MMPS_FS_FILETIME *pTimeStru);
void MMPC_CCIR_OutputEnable(MMP_BOOL on);
void MMPC_Speaker_PowerOn(MMP_BOOL on);
void MMPC_HDMI_DrivePower(MMP_BOOL on);

char    *uniStrcpy(char *dest, const char *src);
char    *strtoUCS(char *dest, const char *src);
char    *strfromUCS(char *dest, const char *src);
size_t  uniStrlen(const short *src);
char    *uniStrcat(char *str1, const char *str2);

void MMPC_Int2Str(MMP_ULONG value, MMP_BYTE *string);
void MMPC_TransfomTime2Stamp(int YY, int MM, int DD, int hh, int mm, int ss, MMP_BYTE* pDate);
void MMPC_AdvanceTime(MMP_TIME_STRUC *pt, MMP_UBYTE ubStep);

// CarDV
void RTNA_LCD_Backlight( MMP_BOOL bEnable );
void MMPC_HDMI_PC_Enable(MMP_BOOL bEnable);

#endif  //_UTILITY_H_


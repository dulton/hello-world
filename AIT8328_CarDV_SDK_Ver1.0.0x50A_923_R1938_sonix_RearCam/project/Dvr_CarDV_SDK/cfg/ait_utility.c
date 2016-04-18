/**
  @file ait_utility.c
  @brief It contains the utility functions need to be ported to the customer platform.
  @author Rogers Chen, Truman

  We should implement regular functions (MMP initialization),
  callback functions (LCD, Audio CODEC initlization) &
  retarget functions (memset, strcat... etc.)

  @version
- 2.0 Add customization functions, such as Bypass pin controller, LCD Initializer
- 1.0 Original version
 */
#ifdef BUILD_CE
#undef BUILD_FW
#endif

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================
#include "customer_config.h"    // CarDV

#include "mmp_lib.h"
#include "ait_utility.h"
#include "mmph_hif.h"
#include "mmp_reg_display.h"
#include "mmp_reg_gbl.h"
#include "lcd_common.h"
#if defined(ALL_FW)
#include "stdarg.h"
#endif
#include "mmpf_pio.h"
#include "AHC_General.h"
#if (USE_PWM_FOR_LCD_BACKLIGHT)
#include "mmpf_pwm.h"
#endif

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

static MMP_UBYTE m_ubDayInMonth[12] = {
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// LCD doesn't have header file
void RTNA_LCD_Init(void);
void RTNA_LCD_RGB_Test(void);
void RTNA_LCD_InitMainLCD(void);
void RTNA_LCD_Init2ndLCD(void);

//==============================================================================
//                              REGULAR FUNCTIONS
//==============================================================================
/**
@brief Customer LCD initialize function.
@retval MMP_TRUE for success, MMP_FALSE for fail
*/
MMP_BOOL MMPC_Display_InitLCD(void)
{
    MMPH_HIF_RegSetB(GBL_LCD_PAD_CFG, MMPH_HIF_RegGetB(GBL_LCD_PAD_CFG) & ~(GBL_LCD_RGB_SPI_MASK));

	switch (RTNA_LCD_GetAttr()->ubDevType)
	{
	case LCD_TYPE_PLCD:
	case LCD_TYPE_PLCD_FLM:
	    MMPH_HIF_RegSetB(GBL_LCD_PAD_CFG, GBL_LCD_PAD_EN);
		break;
	case LCD_TYPE_RGBLCD:
	    MMPH_HIF_RegSetB(GBL_LCD_PAD_CFG, GBL_LCD_PAD_EN | GBL_LCD_RGB_SPI_PAD0);
		break;
	default:
		// NOP
		break;
	}

    RTNA_LCD_Init();

    #ifdef CUS_LCD_BRIGHTNESS
    RTNA_LCD_AdjustBrightness(CUS_LCD_BRIGHTNESS);
    #endif
    #ifdef CUS_LCD_CONTRAST
    RTNA_LCD_AdjustContrast(CUS_LCD_CONTRAST);
    #endif

    #if defined(MBOOT_FW)
    RTNA_LCD_RGB_Test();
    #endif

    return MMP_TRUE;
}

/**
@brief Customer LCD initialize setting(register setting in AIT side) for MAIN LCD.
@retval MMP_TRUE for success, MMP_FALSE for fail
*/
MMP_BOOL MMPC_Display_InitMainLCD(MMP_BOOL bSoftReset)
{
    RTNA_LCD_InitMainLCD();

    return MMP_TRUE;
}

/**
@brief Customer LCD re-initialize setting for MAIN LCD.
@retval MMP_TRUE for success, MMP_FALSE for fail
*/
MMP_BOOL MMPC_Display_ReinitMainLCD(MMP_BOOL bSoftReset)
{
    return MMP_TRUE;
}

#if (defined(ALL_FW) || defined(SD_BOOT))
MMP_USHORT MMPC_FS_FileOpen(MMP_BYTE *bFileName, MMP_BYTE *bMode, void **FileHandle)
{
#if (FS_USE_ICE == 0)
    MMP_USHORT filenamelength;
    #if (FS_INPUT_ENCODE == UCS2)
    MMP_BYTE    ucs2_file[200];
    #endif

    #if (FS_INPUT_ENCODE == UCS2)
    uniStrcpy(ucs2_file, bFileName);
    filenamelength = uniStrlen((short *)ucs2_file);
    #elif (FS_INPUT_ENCODE == UTF8)
    filenamelength = STRLEN(bFileName);
    #endif

    if (MMP_ERR_NONE != MMPS_FS_FileOpen(bFileName, filenamelength, bMode,
                                    STRLEN(bMode), (MMP_ULONG*)FileHandle))
    {
        MMPS_FS_FileClose(*(MMP_ULONG *)FileHandle);
        PRINTF("open file failed.\r\n");
        return 1;
    }
    else {
        return 0;
    }
#else
    *(FILE**)FileHandle = fopen(bFileName, bMode);
    if (*(FILE**)FileHandle == NULL) {
        PRINTF("open %s failed.\r\n", bFileName);
        return 1;
    }
    else {
        return 0;
    }
#endif
}

/** @brief Close file.
*/
MMP_USHORT MMPC_FS_FileClose(void *FileHandle)
{
#if (FS_USE_ICE == 0)
    if (MMP_ERR_NONE != MMPS_FS_FileClose((MMP_ULONG)FileHandle))
    {
        PRINTF("close file failed.\r\n");
        return 1;
    }
    else {
        return 0;
    }
#else
    if (FileHandle == NULL) {
        return 1;
    }
    return fclose((FILE*)FileHandle);
#endif
}

/**
@brief  Seek File(Memory mode)

@param[in] *file_handle : file handle pointer
@param[in] file_offset : Offset for setting the file pointer position
@param[in] file_orign : Mode for positioning file pointer
    -   0 : SEEK_SET
    -   1 : SEEK_CUR
    -   2 : SEEK_END
@return NONE
*/
MMP_USHORT MMPC_FS_FileSeek(void *file_handle, MMP_LONG64 file_offset, MMP_ULONG file_orign)
{
#if (FS_USE_ICE == 0)
    MMP_SHORT origin;

    if(file_orign==0)
        origin = MMPS_FS_SEEK_SET;
    else if(file_orign==1)
        origin = MMPS_FS_SEEK_CUR;
    else
        origin = MMPS_FS_SEEK_END;

    if ( MMP_ERR_NONE != MMPS_FS_FileSeek((MMP_ULONG)file_handle,file_offset,origin) ) {
        return 1;
    }
    else {
        return 0;
    }
#else
    int origin;

    if(file_orign == 0) {
        origin = SEEK_SET;
    }
    else if(file_orign == 1) {
        origin = SEEK_CUR;
    }
    else {
        origin = SEEK_END;
    }

    if (file_handle == NULL) {
        return 1;
    }
    return fseek((FILE*) file_handle, file_offset, origin);
#endif
}

/**
@brief  Write File(Memory mode)

@param[in]  FileHandle  : file handle pointer
@param[in]  uData       : data buffer pointer
@param[in]  ulNumBytes  : write bytes
@param[out] ulWriteCount: actual transfer file size
@return none
*/
MMP_USHORT MMPC_FS_FileWrite(void *FileHandle, MMP_UBYTE *uData, MMP_ULONG ulNumBytes, MMP_ULONG *ulWriteCount)
{
#if (FS_USE_ICE == 0)
    if (MMP_ERR_NONE != MMPS_FS_FileWrite((MMP_ULONG)FileHandle, uData, ulNumBytes, ulWriteCount)) {
        PRINTF("file write failed.\r\n");
        return 1;
    }
    else {
        return 0;
    }
#else
    *ulWriteCount = fwrite(uData, sizeof(MMP_UBYTE), ulNumBytes, (FILE*) FileHandle);

    if (ulNumBytes == *ulWriteCount) {
        return 0;
    }
    else {
        return ferror((FILE*) FileHandle);
    }
#endif
}

/**
@brief  Read File(Memory mode)

@param[in] *FileHandle : file handle pointer
@param[in] *uData : data buffer pointer
@param[in] ulNumBytes : read bytes
@param[out] *ulReadCount : actual transfer file size
@return none
*/
MMP_USHORT MMPC_FS_FileRead(void *FileHandle, MMP_UBYTE *uData, MMP_ULONG ulNumBytes, MMP_ULONG *ulReadCount)
{
#if (FS_USE_ICE == 0)
    if (MMP_ERR_NONE != MMPS_FS_FileRead((MMP_ULONG)FileHandle, uData, ulNumBytes, ulReadCount)) {
        PRINTF("file read failed.\r\n");
        return 1;
    }
    else {
        return 0;
    }
#else
    *ulReadCount = fread(uData, 1, ulNumBytes, (FILE*) FileHandle);

    if (ulNumBytes == *ulReadCount) {
        return 0;
    }
    else {
        return ferror((FILE*) FileHandle);
    }

#endif
}

MMP_USHORT MMPC_FS_FileGetSize(void *FileHandle, MMP_ULONG64 *ulFileSize)
{
#if (FS_USE_ICE == 0)
    if (MMP_ERR_NONE != MMPS_FS_FileGetSize((MMP_ULONG)FileHandle, ulFileSize)) {
        PRINTF("get file size failed.\r\n");
    }
    else {
    }
#else
    long file_pos;

    file_pos = ftell((FILE*) FileHandle);
    fseek((FILE*) FileHandle, 0, SEEK_END);
    *ulFileSize = ftell((FILE*)FileHandle);
    fseek((FILE*) FileHandle, file_pos, SEEK_SET);
#endif

    return 0;
}

/** @brief Return current position of the file pointer.

@param[in] FileHandle The file handler which has been already opened.
@param[out] lFilePos The Current position of file pointer in the file.
@retval 0 Succeed.
@retval others Fail.
*/
MMP_USHORT MMPC_FS_FileTell(void *FileHandle, MMP_LONG64 *llFilePos)
{
#if (FS_USE_ICE == 0)
    return MMPS_FS_FileTell((MMP_ULONG)FileHandle, llFilePos);
#else
    *llFilePos = ftell((FILE*)FileHandle);
    return (*llFilePos == -1);
#endif
}

/** @brief Get Basebnad RTC information to set correct file createion time.
*/
void MMPC_FS_BBGetRTC(MMPS_FS_FILETIME *pTimeStru)
{
#ifdef SD_BOOT   // time structure to be set.
    pTimeStru->usYear = 2006;
    pTimeStru->usMonth = 12;
    pTimeStru->usDay = 31;
    pTimeStru->usHour = 23;
    pTimeStru->usMinute = 55;
    pTimeStru->usSecond = 42;
#else
    // if want to use RTC of baseband, add BB RTC infomation here
    // CraDV, read HW RTC is too slow (8328)
    AHC_RTC_TIME        m_sRTCTime;

    AHC_RTC_GetTime(&m_sRTCTime);

    pTimeStru->usYear = m_sRTCTime.uwYear;
    pTimeStru->usMonth = m_sRTCTime.uwMonth;
    pTimeStru->usDay = m_sRTCTime.uwDay;
    pTimeStru->usHour = m_sRTCTime.uwHour;
    pTimeStru->usMinute = m_sRTCTime.uwMinute;
    pTimeStru->usSecond = m_sRTCTime.uwSecond;
#endif
}
#endif

//==============================================================================
//                              RETARGET FUNCTIONS
//==============================================================================
/** @brief Convert Unicode encoding string to ASCII string.
*/
char *strfromUCS(char *dest, const char *src)
{
    char *dst = dest;

    while(*src != '\0') {
        *dest = *src;
        dest++;
        src+=2;
    }
    *dest = '\0';

    return dst;
}
/** @brief Convert ASCII string to Unicode encoding string.
*/
char *strtoUCS(char *dest, const char *src)
{
    char *dst = dest;

    while(*src != '\0') {
        *dest++ = *src++;
        *dest++ = '\0';
    }
    *dest++ = '\0';
    *dest = '\0';

    return dst;
}
/** @brief Copy a Unicode encoding string.

Copies the string pointed to by src (including the terminating '\0\0')
to the array pointed to by dest. The strings may not overlap,
and the destination string dest must be large enough to receive the copy.
*/
char *uniStrcpy(char *dest, const char *src)
{
    short *s = (short *)src, *d = (short *)dest;

    while(*s != 0) {
        *d++ = *s++;
    }
    *d = 0;

    return dest;
}
/** @brief Calculate the length of a Unicode encoding string.

Calculates the length (in byte) of the Unicode encoding string src,
not including the terminating '\0' character.
*/
size_t uniStrlen(const short *src)
{
    size_t ulLen=0;

    while(*src != '\0') {
        src++;
        ulLen++;
    }

    return (ulLen << 1);
}
/** @brief Concatenate two strings

Appends the Unicode encoding src2 string to the Unicode encoding str1 string
overwriting the '\0\0' at the end of dest, and then adds a terminating '\0\0'.
The strings may not overlap, and the str1 string must have enough space for the result.
*/
char *uniStrcat(char *str1, const char *str2)
{
    short *s = (short *)str2, *d = (short *)str1;

    while(*d != 0) {
        d++;
    }
    while(*s != 0) {
        *d++ = *s++;
    }
    *d = 0;

    return str1;
}

/** @brief Cut off the VDD of SD to power down SD card.

@param[in] none.
@retval none.
*/
MMP_ULONG MMPC_SD_PowerDownMedium(void)
{
    return 0;
}

#if defined(ALL_FW)
/** @brief Power on/off the speaker. (For 8428 HDK)

@param[in] on: 1 to power on speaker; 0 to power off.
@retval none.
*/
void MMPC_Speaker_PowerOn(MMP_BOOL on)
{
#ifdef SPEAKER_ENABLE_GPIO
    if (SPEAKER_ENABLE_GPIO != MMP_GPIO_MAX) {
        if (on) {
            MMPS_PIO_EnableOutputMode(SPEAKER_ENABLE_GPIO, MMP_TRUE);
            MMPS_PIO_SetData(SPEAKER_ENABLE_GPIO, SPEAKER_ENABLE_LEVEL);
        }
        else {
            MMPS_PIO_SetData(SPEAKER_ENABLE_GPIO, !SPEAKER_ENABLE_LEVEL);
        }
    }
#endif
}

/** @brief Turn on/off the CCIR output.

@param[in] on: 1 to enable CCIR output; 0 to disable it.
@retval none.
*/
void MMPC_CCIR_OutputEnable(MMP_BOOL on)
{
#define LCD_GPIO_CAR_BACK           (MMP_GPIO26)    // AGPIO3 -->> LCD_BL
#define LCD_GPIO_CAR_BACK_ACT_LEVEL (GPIO_HIGH)

#ifdef LCD_GPIO_CAR_BACK
    MMPS_PIO_PadConfig(LCD_GPIO_CAR_BACK, PAD_OUT_DRIVING(0));
    MMPS_PIO_EnableOutputMode(LCD_GPIO_CAR_BACK, MMP_TRUE);
    MMPS_PIO_SetData(LCD_GPIO_CAR_BACK, LCD_GPIO_CAR_BACK_ACT_LEVEL);
         
    //MMPS_PIO_EnableOutputMode(MMP_GPIO119, MMP_FALSE); 
    //MMPS_PIO_EnableOutputMode(MMP_GPIO120, MMP_FALSE);
#endif
}


/** @brief HDMI power control. (For 8428 HDK)

@param[in] on: 1 to drive HDMI power; 0 to cut off HDMI power.
@retval none.
*/
void MMPC_HDMI_DrivePower(MMP_BOOL on)
{
#ifdef HDMI_PC_GPIO
    if (HDMI_PC_GPIO != MMP_GPIO_MAX)
    {
        if (on) {
            MMPS_PIO_EnableOutputMode(HDMI_PC_GPIO, MMP_TRUE);
            MMPS_PIO_SetData(HDMI_PC_GPIO, GPIO_HIGH);
        }
        else {
            MMPS_PIO_SetData(HDMI_PC_GPIO, GPIO_LOW);
        }
    }
#endif
}

#endif

/** @brief Convert an integer number to an ASIC string.

@param[in] an interger to be converted.
@param[out] the converted out string.
@retval none.
*/
void MMPC_Int2Str(MMP_ULONG value, MMP_BYTE *string)
{
    MMP_ULONG   i,j;
    MMP_BYTE    temp[16];

    for(i = 0; i < 16; i++)
    {
        if ( (value / 10) != 0 ) {
            temp[i] = '0' + value % 10;

            value = value / 10;
        }
        else {
            temp[i] = '0' + value % 10;
            break;
        }
    }

    for(j = 0; j < (i+1); j++) {
        string[j] = temp[i - j];
    }

    string[j] = 0;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_TransfomTime2Stamp
//  Description :
//------------------------------------------------------------------------------
void MMPC_TransfomTime2Stamp(int YY, int MM, int DD, int hh, int mm, int ss, MMP_BYTE* pDate)
{
    MMP_BYTE StrYear[8], StrMonth[8], StrDay[8], StrHour[8], StrMinute[8], StrSecond[8];

    MMPC_Int2Str(YY, StrYear);
    MMPC_Int2Str(MM, StrMonth);
    MMPC_Int2Str(DD, StrDay);
    MMPC_Int2Str(hh, StrHour);
    MMPC_Int2Str(mm, StrMinute);
    MMPC_Int2Str(ss, StrSecond);

    MEMSET(pDate, 0, 20);

    STRCPY(pDate, StrYear);
    STRCAT(pDate, ":");

    if(MM < 10)
        STRCAT(pDate, "0");

    STRCAT(pDate, StrMonth);
    STRCAT(pDate, ":");

    if(DD < 10)
        STRCAT(pDate, "0");

    STRCAT(pDate, StrDay);

    STRCAT(pDate, " ");

    if(hh < 10)
        STRCAT(pDate, "0");

    STRCAT(pDate, StrHour);
    STRCAT(pDate, ":");

    if(mm < 10)
        STRCAT(pDate, "0");

    STRCAT(pDate, StrMinute);
    STRCAT(pDate, ":");

    if(ss < 10)
        STRCAT(pDate, "0");

    STRCAT(pDate, StrSecond);
}

//------------------------------------------------------------------------------
//  Function    : MMPC_AdvanceTime
//  Description :
//------------------------------------------------------------------------------
void MMPC_AdvanceTime(MMP_TIME_STRUC *pt, MMP_UBYTE ubStep)
{
    pt->uwSecond+= ubStep;
    if (pt->uwSecond < 60)
        return;

    pt->uwSecond = 0;
    pt->uwMinute++;
    if (pt->uwMinute < 60)
        return;

    pt->uwMinute = 0;
    pt->uwHour++;
    if (pt->uwHour < 24)
        return;

    pt->uwHour = 0;
    pt->uwDay++;
    if (pt->uwDay <= m_ubDayInMonth[pt->uwMonth-1])
        return;

    if (pt->uwMonth == 2) {
        if (pt->uwDay == 29) {
            if (pt->uwYear % 4 == 0) {
                if (pt->uwYear % 100 == 0) {
                    if (pt->uwYear % 400 == 0) {
                        // leap year
                        return;
                    }
                } else {
                    // leap year
                    return;
                }
            }
        }
    }

    pt->uwDay = 1;
    pt->uwMonth++;
    if (pt->uwMonth < 13)
        return;

    pt->uwMonth = 1;
    pt->uwYear++;
}

/**
    @brief  
    @return NONE
*/
void RTNA_LCD_Backlight(MMP_BOOL bEnable)
{
#if (USE_PWM_FOR_LCD_BACKLIGHT)
	MMPF_PWM_Initialize();
	MMPF_PWM_OutputPulse(LCD_BACKLIGHT_PWM_UNIT_PIN, bEnable, 6000,50, MMP_TRUE, MMP_FALSE, NULL, 0);
#else
	// (LCD_GPIO_BACK_LIGHT == MMP_GPIOxxx) which can not be used in preprocessor
	#if	defined(LCD_GPIO_BACK_LIGHT)
		if (LCD_GPIO_BACK_LIGHT != MMP_GPIO_MAX)
		{
            MMPF_PIO_PadConfig(LCD_GPIO_BACK_LIGHT, PAD_OUT_DRIVING(0), MMP_TRUE);
			MMPF_PIO_EnableOutputMode(LCD_GPIO_BACK_LIGHT, MMP_TRUE, MMP_TRUE);
		#ifdef LCD_GPIO_BACK_LIGHT_ACT_LEVEL
			MMPF_PIO_SetData(LCD_GPIO_BACK_LIGHT, ((bEnable) ? (LCD_GPIO_BACK_LIGHT_ACT_LEVEL) : !(LCD_GPIO_BACK_LIGHT_ACT_LEVEL)), MMP_TRUE);
		#else
			MMPF_PIO_SetData(LCD_GPIO_BACK_LIGHT, bEnable, MMP_TRUE);
		#endif
		}
	#endif
#endif
}

/**
    @brief  
    @return NONE
*/
void MMPC_HDMI_PC_Enable(MMP_BOOL bEnable)
{
#if defined(HDMI_PC_GPIO)
	if (HDMI_PC_GPIO != MMP_GPIO_MAX) {
        MMPF_PIO_PadConfig(HDMI_PC_GPIO, PAD_OUT_DRIVING(0), MMP_TRUE);
	    MMPF_PIO_EnableOutputMode(HDMI_PC_GPIO, MMP_TRUE, MMP_TRUE);
	    MMPF_PIO_SetData( HDMI_PC_GPIO, bEnable, MMP_TRUE );
	}
#endif
}

#ifdef BUILD_CE
#define BUILD_FW
#endif

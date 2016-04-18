/**
  @file ahc_utility.c
  @brief It contains the utility functions need to be ported to the customer platform.
  @author 

  @version

- 1.0 Original version
 */

#ifdef BUILD_CE
#undef BUILD_FW
#endif

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#if !defined(MBOOT_FW)
#include "Os_wrap.h"
#endif
#include "includes_fw.h"
#include "AIT_Utility.h"
#include "AHC_common.h"
#include "AHC_General.h"
#include "mmpf_uart.h"
#include "lib_retina.h"
#include "AHC_utility.h"
#include "AHC_Fs.h"
#include "MMPF_PIO.H"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

// The setting to check if needs to enter calibration mode
#define ENGNEER_MODE_CALIBRATION_FILE_PATH  "SD:\\CALIBRAT.ENB"
#define ENGNEER_MODE_CALIBRATION_KEY_GPIO   KEYPAD_S06_GPIO//KEYPAD_S08_GPIO

#define SYS_FLAG_DPF_UI        (0x04000000)

#define FS_PATH_MAX_SIZE       256
#define FS_DIVIDE_CHAR         '\\'
/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern MMPF_OS_FLAGID   DSC_UI_Flag;

/*===========================================================================
 * Extern function
 *===========================================================================*/

extern MMP_BOOL UartExecuteCommandEx( char* szCommand );
extern MMP_BOOL UartRegisterUartCmdList( PUARTCOMMAND pUartCmdList );

/*===========================================================================
 * Main body
 *===========================================================================*/

void AHC_OS_Delay(MMP_UBYTE hours, MMP_UBYTE minutes, MMP_UBYTE seconds, MMP_USHORT milli)
{
    OSTimeDlyHMSM(hours,minutes,seconds,milli);
}

UINT8 AHC_WaitFlags(MMP_ULONG ultimeout, MMP_ULONG *flag)
{
    MMP_ERR err = MMP_ERR_NONE;

    err = MMPF_OS_WaitFlags( DSC_UI_Flag, 
                             SYS_FLAG_DPF_UI, 
                             MMPF_OS_FLAG_WAIT_SET_ANY | MMPF_OS_FLAG_CONSUME, 
                             ultimeout, 
                             (MMPF_OS_FLAGS*)flag);

	return err;
}

MMP_ERR AHC_Uart_GetDebugString(MMP_UBYTE uart_id, MMP_BYTE *bDebugString, MMP_ULONG *ulDebugStringLength)
{
	RTNA_DBG_Gets(bDebugString, ulDebugStringLength);
	return MMP_ERR_NONE;
}

AHC_BOOL AHC_UartExecuteCommand( char* szCommand )
{
	return UartExecuteCommandEx( szCommand );
}

AHC_BOOL AHC_UartRegisterUartCmdList( PUARTCOMMAND pUartCmdList )
{
	return UartRegisterUartCmdList( pUartCmdList );
}

AHC_BOOL AHC_UART_Write(char *pWrite_Str, UINT32 ulLength)
{
	#if defined(SA_PL)
	return MMPF_Uart_Write(DEBUG_UART_NUM, pWrite_Str, ulLength);
	#endif
	
	return AHC_TRUE;
}

/*
   AHC_StrLen

Description
    Get the length of a string.

Parameter
    string - Null-terminated string 

Return Value
    Each of these functions returns the number of characters in string, excluding the terminal NULL. No return value is reserved to indicate an error.

*/
unsigned int AHC_StrLen( char *string )
{
    int i=0;
    char* ptr = string;
    
    while( *ptr++ )
    {
        i++;
    }

    return i;
}

/*******************************************************************************
 *
 *   FUNCTION
 *
 *      StrChrReverse
 *
 *   DESCRIPTION
 *
 *      Find the last occurrence of character in str. 
 *
 *   ARGUMENTS
 *
 *       str
 *          C null-end string.
 *       character
 *           Character to be located. It is passed as its int promotion, but it is internally converted back to char for the comparison. 
 *
 *   RETURN
 *   
 *      pointer to the last occurrence of character in str.
 *      If the character is not found, the function returns a null pointer.
 *
*******************************************************************************/
char * StrChrReverse( char * str, char character )
{
    int nLen = AHC_StrLen(str);
    int nIdex;
    nIdex = nLen-1;

    while( nIdex      >=   0 &&
           str[nIdex] != '\0'  )
    {
        if( str[nIdex] == character )
        {
            return str + nIdex;
        }
        nIdex--;
    }

    return NULL;
}


/*******************************************************************************
 *
 *   FUNCTION
 *
 *      GetPathDirStr
 *
 *   DESCRIPTION
 *
 *      Get the dir part of the full Path
 *
 *   ARGUMENTS
 *
 *       szDirName
 *           Output dir path.
 *       nDirBuffMaxSize
 *           The buffer size of szDirName
 *       szFullPath
 *           Input file full Path
 *
 *   RETURN
 *   
 *      The dir part of the path name
 *
*******************************************************************************/
AHC_BOOL GetPathDirStr( char* szDirName, int nDirBuffMaxSize, char* szFullPath )
{
    //INT8            DirName[32];
    char*           ptr;
    int             nPathLength=0;

    memset(szDirName, 0, nDirBuffMaxSize);

    if( AHC_StrLen(szFullPath) > FS_PATH_MAX_SIZE )
    {
        return AHC_FALSE;
    }

    ptr = StrChrReverse(szFullPath, FS_DIVIDE_CHAR);
    if( ptr != NULL )
    {
        nPathLength = ptr - szFullPath;
        if( nPathLength > 0                 && 
            nPathLength < nDirBuffMaxSize   &&
            nPathLength < FS_PATH_MAX_SIZE     )
        {
            memcpy(szDirName, szFullPath, nPathLength );
            return AHC_TRUE;
        }
    }
    return AHC_FALSE;
}


/*
    AHC_Strcmp 

Description
    Compare strings.
        
Parameters
    string1, string2 - Null-terminated strings to compare.
    
Return Value
    
    The return value for each of these functions indicates the lexicographic relation of string1 to string2.

    +--------------------------------------------------+
    |  Return Value   | Description                    |
    +--------------------------------------------------+
    |       < 0       | string1 less than string2      |
    |       0         | string1 identical to string2   |
    |       > 0       | string1 greater than string2   |
    +--------------------------------------------------+    
    
*/

int AHC_Strcmp( const char *string1, const char *string2 )
{
    while( *string1 != NULL && *string2 != NULL )
    {
        if( *string1++ != *string2++ )
        {
            return *string1 - *string2;
        }
    }

    if ( *string1 == NULL || *string2 == NULL ) {
        return (*string1 - *string2);
    }

    return 0;
}

/*
    AHC_Strncmp

Description
    Compare characters of two strings.

Parameters
    string1, string2 - Strings to compare 
    count            - Number of characters to compare 
Return Value
    The return value indicates the relation of the substrings of string1 and string2 as follows.

  +--------------------------------------------------+
  |  Return Value   | Description                    |
  +--------------------------------------------------+
  |       < 0       | string1 less than string2      |
  |       0         | string1 identical to string2   |
  |       > 0       | string1 greater than string2   |
  +--------------------------------------------------+    
*/
int AHC_Strncmp( const char *string1, const char *string2, int count )
{
    int i;
    
    for( i=0; i<count; i++ )
    {
        if( string1[i] != string2[i] )
        {
            return ( string1[i] - string2[i] );
        }
    }

    return 0;
}

/*
    AHC_LowCase

Description
    Convert a character to lower case.

Parameters
    ch - input character to be conveted. 

Return Value
    The character converted. The input character is returned, if the input character is not between 'A' to 'Z'.
    
*/

char AHC_LowCase( const char ch )
{
    if( ch >= 'A' && ch <= 'Z' )
    {
        return ch + ( 'a' - 'A' );
    }
    return ch;
}

/*
    AHC_UpperCase

Description
    Convert a character to upper case.

Parameters
    ch - input character to be conveted. 

Return Value
    The character converted. The input character is returned, if the input character is not between 'a' to 'z'.
    
*/

char AHC_UpperCase( const char ch )
{
    if( ch >= 'a' && ch <= 'z' )
    {
        return ch - ( 'a' - 'A' );
    }
    return ch;
}

/*
    AHC_Stricmp

Description
    Perform a lowercase comparison of strings.

Parameters
    string1, string2 - Null-terminated strings to compare 

Return Value
    
    The return value indicates the relation of string1 to string2 as follows.

    +--------------------------------------------------+
    |  Return Value   | Description                    |
    +--------------------------------------------------+
    |       < 0       | string1 less than string2      |
    |       0         | string1 identical to string2   |
    |       > 0       | string1 greater than string2   |
    +--------------------------------------------------+    
    
*/

int AHC_Stricmp( const char *string1, const char *string2 )
{
    while( *string1 != NULL  && *string2 != NULL )
    {
        char ch1 = AHC_LowCase(*string1);
        char ch2 = AHC_LowCase(*string2);

        if( ch1 != ch2 )
        {
            return ch1 - ch2;
        }
        string1++;
        string2++;        
    }

    return 0;
}

/*
    AHC_Strincmp

Description
    Perform a lowercase comparison of characters.

Parameters
    string1, string2 - Strings to compare 
    count            - Number of characters to compare 
Return Value
    The return value indicates the relation of the substrings of string1 and string2 as follows.

  +--------------------------------------------------+
  |  Return Value   | Description                    |
  +--------------------------------------------------+
  |       < 0       | string1 less than string2      |
  |       0         | string1 identical to string2   |
  |       > 0       | string1 greater than string2   |
  +--------------------------------------------------+    
*/

int AHC_Strincmp( const char *string1, const char *string2, int count )
{
    int i;
    
    for( i=0; i<count; i++ )
    {
        char ch1 = AHC_LowCase(string1[i]);
        char ch2 = AHC_LowCase(string2[i]);

        if( ch1 != ch2 )
        {
            return ch1 - ch2;
        }
    }

    return 0;
}

/*
    DramToFile

Description
    Save a buffer into a file.

Parameters
    szFilename  - the file name
    pbyBuffer   - the memory buffer address
    ulSize      - size to save
Return Value
    AHC_TRUE    - Successful
    AHC_FALSE   - Failed

*/

AHC_BOOL DramToFile( INT8* szFilename, INT8* pbyBuffer, UINT32 ulSize )
{
    UINT32 ulFileId;
    UINT32 ulActualSize;
    
    if (AHC_FS_FileOpen(szFilename, AHC_StrLen(szFilename), "wb", AHC_StrLen("wb"),  &ulFileId )) 
    {
        return AHC_FAIL;
    }
    else 
    {
        AHC_FS_FileWrite(ulFileId, (UINT8 *)pbyBuffer, ulSize, &ulActualSize);
        AHC_FS_FileClose(ulFileId) ;
    }
    
    return AHC_TRUE;
}

/*
    FileToDram

Description
    Load a file into a buffer.

Parameters
    szFilename  - the file name
    pbyBuffer   - the memory buffer address
    ulSize      - size to save
Return Value
    AHC_TRUE    - Successful
    AHC_FALSE   - Failed

*/

AHC_BOOL FileToDram( INT8* szFilename, INT8* pbyBuffer, UINT32 ulFilePos, UINT32 ulSize )
{
    UINT32 ulFileId;
    UINT32 ulActualSize;
    
    if (AHC_FS_FileOpen(szFilename, AHC_StrLen(szFilename), "rb", AHC_StrLen("rb"),  &ulFileId )) 
    {
        return AHC_FAIL;
    }
    
    if( AHC_ERR_NONE != AHC_FS_FileSeek(ulFileId, ulFilePos, AHC_FS_SEEK_SET) )
    {
        return AHC_FAIL;
    }

    if( AHC_ERR_NONE != AHC_FS_FileRead(ulFileId, (UINT8 *)pbyBuffer, ulSize, &ulActualSize) )
    {
        return AHC_FAIL;
    }

    AHC_FS_FileClose(ulFileId) ;

    return AHC_TRUE;
}

/*
    AHC_IsFileExist

Description
    Check if a file is exist.

Parameters
    szFilename  - the file name

Return Value
    AHC_TRUE    - Exist 
    AHC_FALSE   - Not exist.

*/

AHC_BOOL AHC_IsFileExist( INT8* szFilename )
{
    UINT32    file_id = 0;
    AHC_BOOL  bResult = AHC_TRUE;

    if (AHC_FS_FileOpen((INT8 *)szFilename, 
                        AHC_StrLen(szFilename), 
                        (INT8*)"rb",
                        2, 
                        &file_id) != AHC_ERR_NONE )
    {
        bResult = AHC_FALSE;
    }

    AHC_FS_FileClose(file_id);
    printc("AHC_IsFileExist bResult=%d \n",bResult);
    
    return bResult;
}



/*
    AHC_Strrchr

Description
    Locale the last occurence of the certain character c in the string pointed by s.

Parameters
    s  - the string to be searched.
    c  - the char to find.

Return Value
    NULL     - Not found
    NON-NULL - the location of the found character


*/

char *AHC_Strrchr(char *s, char c)
{
    UINT nLen = AHC_StrLen(s);
    char* pFound = NULL;

    if( c == '\0' )
    {
        return s+nLen;
    }
 
    while(nLen>0)
    {
        nLen--;
        if( s[nLen] == c )
        {
            pFound = &s[nLen];
            break;
        }
    }
    
    return pFound;
}


/*
    AHC_CheckEngineerMode

Description
    Check if a engineer mode is enable by a specific file is exist & a
  	Specific GPIO is in some state.

Parameters
    szFilename  - the file name
    pioKeyPress - GPIO pin
    bPressState - GPIO press state

Return Value
    AHC_TRUE    - Engineering mode. 
    AHC_FALSE   - Normal mode 

*/
AHC_BOOL AHC_CheckEngineerMode( INT8* szFilename, AHC_PIO_REG pioKeyPress)
{
    UINT8 bCurrState;

    if(AHC_ConfigGPIO( pioKeyPress, 0 ) == AHC_FALSE){// Input mode
    	return AHC_FALSE;
    }
    
    AHC_GetGPIO( pioKeyPress, &bCurrState );

	if(  AHC_IsFileExist(szFilename) )
    {
        return AHC_TRUE;
    }

    return AHC_FALSE;
}

AHC_BOOL AHC_IsCalibrationMode(void)
{
    static AHC_BOOL bFirstTime = AHC_TRUE;
    static AHC_BOOL bCaliMode  = AHC_FALSE;

    if( bFirstTime )
    {
        bFirstTime = AHC_FALSE;
        bCaliMode  = AHC_CheckEngineerMode( ENGNEER_MODE_CALIBRATION_FILE_PATH, 
                                            ENGNEER_MODE_CALIBRATION_KEY_GPIO); 
                                             
        if( bCaliMode )
            printc("Enter [Cali] mode bCaliMode=%d\n",bCaliMode);
    }
    return bCaliMode;    
}

INT32 AHC_Strcasecmp(const INT8 *s1, const INT8 *s2)
{
	char	c1, c2;
	int		result = 0;

	while (result == 0)
	{
		c1 = *s1++;
		c2 = *s2++;
		if ((c1 >= 'a') && (c1 <= 'z'))
			c1 = (char)(c1 - ' ');
		if ((c2 >= 'a') && (c2 <= 'z'))
			c2 = (char)(c2 - ' ');
		if ((result = (c1 - c2)) != 0)
			break;
		if ((c1 == 0) || (c2 == 0))
			break;
	}
	return result;
}

#ifdef BUILD_CE
#define BUILD_FW
#endif

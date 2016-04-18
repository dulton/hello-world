//==============================================================================
/**
 @file AHC_DCFDT.c
 @brief Date Time Dcf 
 @author Caesar Chang
 @version 1.0
*/



/*===========================================================================
 * Include files
 *===========================================================================*/
#include "includes_fw.h"
#include "Menusetting.h"
#include "AHC_Common.h"
#include "AHC_Config_SDK.h"
#include "AHC_DateTime.h"
#include "AHC_UF.h"
#include "AHC_DCFDT.h"
#include "AHC_DCF.h"
#include "ait_utility.h"
#include "AHC_utility.h"
#include "AHC_FS.h"
#include "AHC_MACRO.h"
#include "AHC_MemPoolFixedSize.h"

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)

extern AHC_BOOL AHC_GetStorageMediaInfo(AHC_FS_DISK_INFO* sMediaInfo);

void* AHC_DCFDT_FileNodeAlloc( void );
void AHC_DCFDT_AddFile(PSDATETIMEDCFDB pDB, char* szFilename, UINT32 ulSize, AHC_FS_ATTRIBUTE sFileAttrib );
void AHC_DCFDT_AddRearFile(PSDATETIMEDCFDB pDB, char* szFilename, UINT32 ulSize, AHC_FS_ATTRIBUTE sFileAttrib );
static AHC_BOOL AIHC_DCFDT_GetMediaInfo(PSDATETIMEDCFDB pDB);
AHC_BOOL AHC_DCFDT_LockInit(PSDATETIMEDCFDB pDB);
AHC_BOOL AHC_DCFDT_LockUninit(PSDATETIMEDCFDB pDB);
AHC_BOOL AHC_DCFDT_LockEx(PSDATETIMEDCFDB pDB, UINT32 uiTimeOutTick );
AHC_BOOL AHC_DCFDT_Lock(PSDATETIMEDCFDB pDB );
AHC_BOOL AHC_DCFDT_Unlock(PSDATETIMEDCFDB pDB);
AHC_BOOL AHC_DCFDT_DbPreAddFile(PSDATETIMEDCFDB pDB, char* szFilename, AHC_BOOL bDelDuplicated );
AHC_BOOL AHC_DCFDT_DbPostAddFile(PSDATETIMEDCFDB pDB, char* szFilename);
PSDATETIMEFILE AIHC_DCFDT_GetNodeByIndex(PSDATETIMEDCFDB pDB, UINT uiIndex );
void* AHC_DCFDT_FileNodeAlloc( void );
AHC_BOOL AHC_DCFDT_FileNodeFree( void* pbyBuff );
AHC_BOOL AHC_DCFDT_GetNameByNode(PSDATETIMEDCFDB pDB, PSDATETIMEFILE pNode, char* szNameBuff, UINT uiBuffLength );
PSDATETIMEDCFDB AHC_DCFDT_GetDBByFolderName(char* szFolderName);
AHC_BOOL AHC_DCFDT_GetFullPathByNode(PSDATETIMEDCFDB pDB, PSDATETIMEFILE pNode, char* szNameBuff, UINT uiBuffLength );

AHC_BOOL AHC_DCFDT_AdjustAllocation(DCF_STORAGE_ALLOC eType);
AHC_BOOL AHC_DCFDT_DbRemoveFileByNode(PSDATETIMEDCFDB pDB, PSDATETIMEFILE  psNodeCurr, AHC_BOOL bDeleteInFS );
//for only DCFDT use
void AIHC_DCFDT_UpdateLockFileTable(PSDATETIMEFILE psNode, AHC_BOOL *bDeleteFile, UINT8 byOperation);

AHC_UF_Callback        *AHC_DCFDT_CB_GetStorageAllocationSetting = NULL;
AHC_BOOL UI_WriteSdUuidXML(void);
int sprintc( char* szOutput, char* szFormat, ... );
/*******************************************************************************
 *
 *   DCFDT 
 *
*******************************************************************************/
#define MAX_DCF_SUPPORT_FILE_TYPE   	15
#define NUM_OF_DCFDT_DB				    4

static const INT8 			m_DcfSupportFileType[MAX_DCF_SUPPORT_FILE_TYPE][4]  = {	"JPG","TIF","AVI",
                                                                            "MOV","MP4","3GP",
                                                                            "WMV","MP3","OGG",
                                                                            "WMA","WAV","ASF",
                                                                            "THM","RAW"};

static AHC_FS_DISK_INFO     m_sMediaInfo;
static UINT32		        m_uiBytesPerCluster = 0;
static AHC_BOOL             m_bSetRearCam = AHC_FALSE;
static AHC_BOOL             m_bCreateRearCamFile =AHC_FALSE;
PSMEMPOOLFIXEDSIZE          hFileNodeMemPool;
//SDATETIMEDCFDB              m_sDB[NUM_OF_DCFDT_DB]={0};
static DCF_STORAGE_ALLOC m_AllocType = DCF_STORAGE_ALLOC_TYPE0;

typedef struct tagDCFDTSETTING
{
  PSMEMPOOLFIXEDSIZE        m_hFileNodeMemPool;
  SDATETIMEDCFDB 		    m_sDB[DCF_DB_COUNT];
  AHC_BOOL                  m_bDBEnable[DCF_DB_COUNT];
  AHC_BOOL                  m_bMountDCF[DCF_DB_COUNT];  
  UINT                      m_uiCurrDBIdx;
  DCF_DB_TYPE               m_sDBType;

//  AHC_FS_DISK_INFO          m_sMediaInfo;
//  UINT32		            m_uiBytesPerCluster;
} SDCFDTSETTING,*PSDCFDTSETTING;

static SDCFDTSETTING sDcfDtSet=
{
  NULL,            // PSMEMPOOLFIXEDSIZE          hFileNodeMemPool;
  {0},             // SDATETIMEDCFDB              m_sDB[NUM_OF_DCFDT_DB]
  {
    AHC_TRUE,
    #if ( DCF_DB_COUNT >= 2 )
    AHC_TRUE,    
    #endif
    #if ( DCF_DB_COUNT >= 3 )
    AHC_TRUE,
    #endif
    #if ( DCF_DB_COUNT >= 4 )
    AHC_TRUE,    
    #endif
  },               // AHC_BOOL                    m_bDBEnable[DCF_DB_COUNT];
  {0},             // AHC_BOOL                    m_bMountDCF[DCF_DB_COUNT];
  0,               // UINT                        m_uiCurrDBIdx;
  0,               // DCF_DB_TYPE                 m_sDBType;

//  {0},             // AHC_FS_DISK_INFO            m_sMediaInfo;
//  512,             // UINT32		              m_uiBytesPerCluster;
  
};

/*******************************************************************************
 *
 *   START DATETIME
 *
*******************************************************************************/
/*

//#define MAX_DCF_SUPPORT_FILE_TYPE  (sizeof(m_DcfSupportFileType)/(char*))

     char 		m_DcfAllowedFileType[MAX_DCF_SUPPORT_FILE_TYPE]     = {	1, 1, 1,
                                                                            1, 1, 1,
                                                                            1, 1, 1,
                                                                            1, 1, 1,
                                                                            1, 1    };
*/


static AHC_BOOL AIHC_DCFDT_CreateFilename( PSDATETIMEDCFDB pDb, DATE_TIME nDateTime, BYTE byFileType, char* szFilename )
{
    AHC_RTC_TIME sRtcTime={0};
    
    if( AHC_DT_SecondsToDateTime(nDateTime, &sRtcTime ))
    {
        INT8 szTemplate[DCF_MAX_FILE_NAME_SIZE];
        if(m_bSetRearCam == AHC_FALSE)
        {
            MEMCPY(szTemplate, pDb->szTemplate, sizeof(pDb->szTemplate));
        }
        else
        {
            MEMCPY(szTemplate, pDb->szTemplate_2nd, sizeof(pDb->szTemplate_2nd));
        }
        sprintc( szFilename, szTemplate, sRtcTime.uwYear - DCFDT_FILENAME_YEAR_OFFSET
                                       , sRtcTime.uwMonth
                                       , sRtcTime.uwDay
                                       , sRtcTime.uwHour
                                       , sRtcTime.uwMinute
                                       , sRtcTime.uwSecond );
    
        STRCAT( szFilename, "." );
        // Ext
        if( byFileType >= MAX_DCF_SUPPORT_FILE_TYPE )
        {
            return AHC_FALSE;

        }
        STRCAT( szFilename, m_DcfSupportFileType[byFileType] );    
        return AHC_TRUE;
    }
    return AHC_FALSE;
}

static AHC_BOOL AIHC_DCFDT_DeleteFileInFS( char* szFullPath, AHC_BOOL bIsFileReadOnly )
{
#if FS_FORMAT_FREE_ENABLE
    {
        AHC_FS_ATTRIBUTE attrib;
        AHC_FS_FileDirGetAttribute(szFullPath, 
                                       STRLEN(szFullPath), &attrib );
        attrib |= AHC_FS_ATTR_HIDDEN;
        if( bIsFileReadOnly )
        {
            attrib &= ~(AHC_FS_ATTR_READ_ONLY);
        }
        AHC_FS_FileDirSetAttribute(szFullPath, 
                                       STRLEN(szFullPath), attrib );
    }
#else
    if( bIsFileReadOnly )
    {
        // Remove read only attribute
        AHC_FS_ATTRIBUTE attrib;
        AHC_FS_FileDirGetAttribute(szFullPath, 
                                   STRLEN(szFullPath), &attrib );

        attrib &= ~(AHC_FS_ATTR_READ_ONLY|AHC_FS_ATTR_HIDDEN|AHC_FS_ATTR_SYSTEM);
        AHC_FS_FileDirSetAttribute(szFullPath, 
                                   STRLEN(szFullPath), attrib );
                                   
    }
      
    if( AHC_ERR_NONE != AHC_FS_FileRemove(szFullPath, 
                                          STRLEN(szFullPath)))
    {
        printc("Remove file %s failed!\n", szFullPath );
        return AHC_FALSE;
    }
#endif
    printc("Delete %s\n", szFullPath);

    return AHC_TRUE;

}


static AHC_BOOL AIHC_DCFDT_ChangeToNmeaName(char* szFilePath )
{
    char* ptr;

    // Change the extention name to .NMEA file path
    // Rear find the char '.'
    ptr = AHC_Strrchr(szFilePath, '.' );
    if( ptr == NULL )
    {
        return AHC_FALSE;
    }

    // Make path end at '.'
    ptr++;
    *ptr = '\0';

    // Paste 'NMEA' at the end.
    STRCAT(ptr, DCFDT_NEMA_FILE_EXTENTION);

    return AHC_TRUE;
}


static AHC_BOOL AIHC_DCFDT_MakeNmeaName(char* szFilePath, char* szNmeaFilePath )
{
    char* ptr;

    if( szNmeaFilePath == NULL )
    {
        return AHC_FALSE;
    }

    STRCPY( szNmeaFilePath, szFilePath );

    // Change the extention name to .NMEA file path
    // Rear find the char '.'
    ptr = AHC_Strrchr(szNmeaFilePath, '.' );
    if( ptr == NULL )
    {
        return AHC_FALSE;
    }

    // Make path end at '.'
    ptr++;
    *ptr = '\0';

    // Paste 'NMEA' at the end.
    STRCAT(ptr, DCFDT_NEMA_FILE_EXTENTION);

    return AHC_TRUE;
}

static AHC_BOOL AIHC_DCFDT_DeleteNmeaFileInFS( char* szFullPath, AHC_BOOL bIsFileReadOnly )
{

    char  szNmeaFilePath[DCF_MAX_FILE_NAME_SIZE];

    // Get NMEA file path
    if( !AIHC_DCFDT_MakeNmeaName(szFullPath, szNmeaFilePath ) )
    {
        return AHC_FALSE;
    }
    
    if( bIsFileReadOnly )
    {
        // Remove read only attribute
        AHC_FS_ATTRIBUTE attrib;
        AHC_FS_FileDirGetAttribute(szNmeaFilePath, 
                                   STRLEN(szNmeaFilePath), &attrib );

        attrib &= ~(AHC_FS_ATTR_READ_ONLY|AHC_FS_ATTR_HIDDEN|AHC_FS_ATTR_SYSTEM);
        AHC_FS_FileDirSetAttribute(szNmeaFilePath, 
                                   STRLEN(szNmeaFilePath), attrib );
                                   
    }

    if( AHC_ERR_NONE != AHC_FS_FileRemove(szNmeaFilePath, 
                                          STRLEN(szNmeaFilePath)))
    {
        printc("Remove Nmea file %s failed!\n", szNmeaFilePath );
        return AHC_FALSE;
    }

    printc("Delete %s\n", szNmeaFilePath);
    return AHC_TRUE;
}

/**************************************************************************************************

DCFDT

***************************************************************************************************/
AHC_BOOL AHC_DCFDT_RegisterCallback(MMP_ULONG ulCBType, void *pCallback)
{
    MMP_ERR ret = MMP_ERR_NONE;

    switch(ulCBType) {
    case AHC_UFCB_GET_STORAGE_ALLOCATION_SETTING:
        AHC_DCFDT_CB_GetStorageAllocationSetting = (AHC_UF_Callback *)pCallback;
    break;
    default:
        ret = MMP_SYSTEM_ERR_PARAMETER;
    }
    return ret;
}		

void AHC_DCFDT_InitDB(PSDATETIMEDCFDB pDB, char* szFolder, char* szTemplate, UINT uiFilenameLen, UINT uiFlag)
{
    MEMSET(pDB, 0, sizeof(SDATETIMEDCFDB) );
    // Init
    strcpy( pDB->szFolderName, szFolder );

    strcpy( pDB->szTemplate, szTemplate);

    pDB->ulFilenameLen = uiFilenameLen;

    pDB->uiFlag = uiFlag;
    
    //pDB->uiCurrentIndex = pDB->ulTotalObjectNum - 1; //Patch from Mark

    if( uiFlag & DCFDT_DB_FLAG_COUNT_PAYLOAD_INTO_TH )
    {
        pDB->bThreadConsiderPayload = AHC_TRUE;
    }

    // Read file system cluster info
    // Get Info
    AIHC_DCFDT_GetMediaInfo(pDB);
    // If folder is not exist, create it.
}

void AHC_DCFDT_InitRearCamPara(PSDATETIMEDCFDB pDB, char* szFolder, char* szTemplate)
{

    strcpy( pDB->szFolderName_2nd, szFolder );
    strcpy( pDB->szTemplate_2nd, szTemplate );
}
AHC_BOOL AHC_DCFDT_ParseFolder( PSDATETIMEDCFDB pDB )
{
    #define             DCF_FOLDER_MAX_LEVEL  3
    #define             DCF_DIVIDE_CHAR       '\\'
    char*               szFolder;

    UINT32              error;

    UINT32              uiFolderID;
    AHC_FS_FILETIME		FileTimeStruct;
    UINT32              uiFileSize;
    UINT32              uiNameLength;
    UINT8               byFileMode;    
    AHC_FS_ATTRIBUTE    sFileAttrib;
    char                szFilename[DCF_MAX_FILE_NAME_SIZE];
    char*               ptr;
    INT8                byFolderLevel = 0;
    int                 i=0,j=0; 
    
    byFileMode      =   AHC_FILE_NAME | AHC_ATTRIBUTE | AHC_ENTRY_SIZE;
    szFolder        =   pDB->szFolderName;
    uiNameLength    =   STRLEN(szFolder);
    //Calculate Folder Level
    i = STRLEN(szFolder) - 1;
    while( i >= 0 && szFolder[i] != '\0')
    {
        if( szFolder[i] == DCF_DIVIDE_CHAR )
        {
            byFolderLevel++;
        }
        i--;
    }

    // Open directory for each level
    for(i=0; i<byFolderLevel; i++)
    {
        char szTempFolderPath[DCF_MAX_FILE_NAME_SIZE];
        char szDirFolderPath[DCF_MAX_FILE_NAME_SIZE];
        UINT8 byPathLength = 0;
        UINT8 byTempLength = uiNameLength;
        UINT8 byLevelCount = byFolderLevel - 1 - i;
        AHC_BOOL bCreateFile = AHC_FALSE;
        memset(szDirFolderPath, 0, sizeof(szDirFolderPath));
        for(j=0; j<byLevelCount; j++)
        { 
            memset(szTempFolderPath, 0, sizeof(szTempFolderPath));
            memcpy(szTempFolderPath, szFolder, byTempLength);
            ptr = StrChrReverse(szTempFolderPath,DCF_DIVIDE_CHAR);
            byTempLength = ptr - szTempFolderPath;
            byPathLength += (STRLEN(szTempFolderPath) - byTempLength);
        }
        byPathLength = uiNameLength - byPathLength;
        memcpy(szDirFolderPath, szFolder, byPathLength);
        //printc("szDirFolderPath %s\r\n",szDirFolderPath);
        if(AHC_FS_DirOpen(szDirFolderPath, byPathLength, &uiFolderID) != MMP_ERR_NONE)
        {
            AHC_FS_DirCreate(szDirFolderPath, byPathLength);       ///< Create a new directory
            bCreateFile = AHC_TRUE;
        }
        else
        {
            if(i != byFolderLevel -1 )
            AHC_FS_DirClose(uiFolderID);
        }
        
        if(i == (byFolderLevel - 1))
        {
            if(bCreateFile == AHC_TRUE)
            {
                return AHC_FALSE;
            }
        }
    }
    
    while(1)
    {
        error = AHC_FS_GetFileList( uiFolderID,
                                 byFileMode,
                                 szFilename,
                                 &sFileAttrib,
                                 &FileTimeStruct,
                                 &uiFileSize );

        if ( AHC_FS_ERR_NO_MORE_ENTRY == error ){
            /**
            @brief no file in this dir
            */
            break;
        }

        // Skip first two items
        if( szFilename[0] == '.')
        {
            continue;
        }

        if(AHC_FS_ATTR_DIRECTORY & sFileAttrib) 
        {
            // Folder
        }
        else
        {
            // Added file
            AHC_DCFDT_AddFile(pDB, szFilename, uiFileSize, sFileAttrib);
/*
            AHC_DCFDT_DbPreAddFile(pDB, szFilename, AHC_TRUE);
            AHC_DCFDT_DbPostAddFile(pDB, szFilename);
*/

#if (DCFDT_DEBUG)
            printc("[%d]%s\n", i, szFilename);
#endif
            i++;
        }

    }

    error = AHC_FS_DirClose(uiFolderID);	
    
    return TRUE;
}

AHC_BOOL AHC_DCFDT_ParseRearCamFolder( PSDATETIMEDCFDB pDB )
{
    char*               szFolder;

    UINT32              error;

    UINT32              uiFolderID;
    AHC_FS_FILETIME		FileTimeStruct;
    UINT32              uiFileSize;
    UINT32              uiNameLength;
    UINT8               byFileMode;    
    AHC_FS_ATTRIBUTE    sFileAttrib;
    char                szFilename[DCF_MAX_FILE_NAME_SIZE];

    byFileMode      =   AHC_FILE_NAME | AHC_ATTRIBUTE | AHC_ENTRY_SIZE;
    szFolder        =   pDB->szFolderName_2nd;  
    uiNameLength    =   STRLEN(szFolder);

    // Open directory
    if(AHC_FS_DirOpen(szFolder, uiNameLength, &uiFolderID) != MMP_ERR_NONE){
        AHC_FS_DirCreate(szFolder, uiNameLength);       ///< Create a new directory.   //Rogers:Add 
        return AHC_FALSE;
    }

    while(1)
    {
        error = AHC_FS_GetFileList( uiFolderID,
                                 byFileMode,
                                 szFilename,
                                 &sFileAttrib,
                                 &FileTimeStruct,
                                 &uiFileSize );

        if ( AHC_FS_ERR_NO_MORE_ENTRY == error ){
            /**
            @brief no file in this dir
            */
            break;
        }

        // Skip first two items
        if( szFilename[0] == '.')
        {
            continue;
        }

        if(AHC_FS_ATTR_DIRECTORY & sFileAttrib) 
        {
            // Folder
        }
        else
        {
            //Added file
            AHC_DCFDT_AddRearFile(pDB, szFilename, uiFileSize, sFileAttrib);
        }

    }

    error = AHC_FS_DirClose(uiFolderID);	
    
    return TRUE;
}

void AHC_DCFDT_UnmountDB(PSDATETIMEDCFDB pDB)
{
    // free all file unit
    // clear db

    AHC_DCFDT_LockUninit(pDB);

    MEMSET( pDB, 0, sizeof(SDATETIMEDCFDB) );

}

void AHC_DCFDT_MountDB(PSDATETIMEDCFDB pDB)
{
    // Check if umount yet
    // If not, umount it.
    AHC_DCFDT_LockInit(pDB);

    // Get Info
    AIHC_DCFDT_GetMediaInfo(pDB);

    // Add file to db
    // update db stastics data
    AHC_DCFDT_ParseFolder(pDB);
#if DCF_DUAL_CAM_ENABLE
    AHC_DCFDT_ParseRearCamFolder(pDB);
#endif
}


AHC_BOOL AHC_DCFDT_GetFileExtension(char* szFilename, char* szExt, int nExtSize )
{
    int nLength = STRLEN(szFilename);
    int nTmp    = nLength;
    int i;

    // find dot
    while( nTmp && szFilename[nTmp] != '.' )
    {
        nTmp--;
    }

    if(nTmp==0) return AHC_FALSE;

    nTmp++;

    if( nExtSize < nLength - nTmp )
    {
        return AHC_FALSE;
    }

    nExtSize = nLength - nTmp;
    
    for( i=0; i< nExtSize; i++ )
    {
        szExt[i] = szFilename[nTmp+i];
    }

    szExt[i] = '\0';

#if (DCFDT_DEBUG)
    printc("szExt=%s\n",szExt );
#endif

    return AHC_TRUE;

}

BYTE AHC_DCFDT_GetFileExtType(char* szExt )
{
    int i;
    int type = DCFDT_UNKNOW_TYPE;
    for( i=0; i< MAX_DCF_SUPPORT_FILE_TYPE; i++ )
    {
        if( STRCMP( m_DcfSupportFileType[i], szExt ) == 0 )
        {
            type = i;
            break;
        }
    }

    return type;
}


UINT8 AHC_DCFDT_GetFileType(char* szFilename)
{
    #define FILE_EXTENSION_SIZE     5
    char szExt[FILE_EXTENSION_SIZE]={0};
    BYTE  type;
    
    if( AHC_DCFDT_GetFileExtension(szFilename, szExt, FILE_EXTENSION_SIZE ) == AHC_FALSE )
    {
        return (UINT8) DCFDT_UNKNOW_TYPE;
    }
    
    // Get file type
    type = AHC_DCFDT_GetFileExtType(szExt);

    return type;
}

void AHC_DCFDT_DumpDb(PSDATETIMEDCFDB pDB)
{

#if 1 //(DCFDT_DEBUG)
    PSDATETIMEFILE psNode = pDB->pFileHead;
    INT            n=0;
    UINT           uiTotalClusters =0;
    UINT           uiTotalFileNum  =0;
    UINT           uiClustersPerMB = 1024*1024 / pDB->ulClusterSize;
    printc("%s\n", pDB->szFolderName);
    while(psNode)
    {
        char szFilename[DCF_MAX_FILE_NAME_SIZE];

        uiTotalClusters += psNode->nSizeInCluster;
        uiTotalFileNum  ++;
        AHC_DCFDT_GetNameByNode(pDB, psNode, szFilename, DCF_MAX_FILE_NAME_SIZE);        

        printc("\t%s Ty%d R%d %d CL%d RE", szFilename, 
                                           psNode->byFileType, 
                                           psNode->bReadOnly, 
                                           psNode->nSizeInCluster * pDB->ulClusterSize, 
                                           psNode->bCharLock, 
                                           psNode->bIsRearFileExist);
        #if (FS_FORMAT_FREE_ENABLE)
        printc("\tHidden? %d",psNode->bHidden);
        #endif
        //psNode.nDateTime
        #if ENABLE_DATE_VALUE_DEBUG
        printc("\tFILE  %d-%d-%d %d:%d:%d", psNode->uwYear,
                                            psNode->uwMonth  ,
                                            psNode->uwDay   ,
                                            psNode->uwHour  ,
                                            psNode->uwMinute,
                                            psNode->uwSecond );
        #endif
        printc("\n");
        n++;
        psNode = psNode->pNext;
    }

    printc( "Cluster Size=%d\n", pDB->ulClusterSize );
    printc( "Threshold   =%d (%dMB) \n", pDB->ulMaxSizeThresholdInCluster, pDB->ulMaxSizeThresholdInCluster/uiClustersPerMB);
    printc( "Payload     =%d (%dMB)\n", pDB->ulPayloadInCluster, pDB->ulPayloadInCluster/uiClustersPerMB);

    printc( "Number of Objects %d \n", pDB->ulTotalObjectNum );
    printc( "Number of Rear Cam Objects %d \n", pDB->ulTotalRearCamObjectNum);
    printc( "Total Size of Objects %d (%dMB)\n", (UINT32)(pDB->ullTotalObjectSize / pDB->ulClusterSize), (UINT32)(pDB->ullTotalObjectSize/1024/1024)  );

    if( uiTotalFileNum != pDB->ulTotalObjectNum )
    {
        printc( "Sum of total files num      %d\n", uiTotalFileNum );	
    }

    if( uiTotalClusters != pDB->ullTotalObjectSize / pDB->ulClusterSize )
    {
        printc( "Sum of total files clusters %d\n", uiTotalClusters );
    }
#endif
}


static AHC_BOOL AIHC_DCFDT_GetMediaInfo(PSDATETIMEDCFDB pDB)
{

    UINT64 uiiTotalBytes;
    
    // Get the info of media
    AHC_GetStorageMediaInfo(&m_sMediaInfo);

    m_uiBytesPerCluster = m_sMediaInfo.usBytesPerSector*m_sMediaInfo.usSecPerCluster;

    
    uiiTotalBytes = (UINT64)m_sMediaInfo.ulTotalClusters*m_uiBytesPerCluster;
    
    pDB->ulClusterSize    = ((ULONG)m_sMediaInfo.usBytesPerSector)*m_sMediaInfo.usSecPerCluster;
    pDB->ullDiskClusters  = m_sMediaInfo.ulTotalClusters;
    
#if (DCFDT_DEBUG)
    printc("BytesPerSec     : %d, \r\n", m_sMediaInfo.usBytesPerSector);
    printc("SecPerCluster   : %d, \r\n", m_sMediaInfo.usSecPerCluster);
    printc("TotalCluster    : %d, \r\n", m_sMediaInfo.ulTotalClusters);
    printc("FreelCluster    : %d, \r\n", m_sMediaInfo.ulFreeClusters);	
    printc("BytesPerCluster : %d, \r\n", m_uiBytesPerCluster);
    printc("uiiTotalBytes    : %d, \r\n", uiiTotalBytes);
#endif	
    
    return AHC_TRUE;

}

void AHC_DCFDT_AddFile(PSDATETIMEDCFDB pDB, char* szFilename, UINT32 ulSize, AHC_FS_ATTRIBUTE sFileAttrib )
{
    int var[6];
    PSDATETIMEFILE psNode;
    UINT8          byFileType;
    ULONG          ulSizeAligned;
    ULONG          ulLen;
    AHC_BOOL       bIsCharLock = AHC_FALSE; //default
    ulLen = STRLEN(szFilename);

    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        printc("AHC_DCFDT_AddFile: Unknow Type %s\n", szFilename );
        return;
    }
    
#if 0 //Rogers: Test Del
    // TBD: Check if byFileType is allowed, if not just return
    if( pDB->chAllowedFileType[byFileType] == 0 )
    {
        printc("AHC_DCFDT_AddFile: Fail02\r\n");
        return;
    }
#endif
    //Compare CHARLOCK template
    if(AHC_DCFDT_TemplateCmp( szFilename, DCFDT_CHARLOCK_TEMPLATE))
    {
        bIsCharLock = AHC_TRUE;
        //printc("find char lock file %s\r\n", szFilename);
        sscanfl( szFilename, DCFDT_CHARLOCK_TEMPLATE,&var[0]
                                                    ,&var[1]
                                                    ,&var[2]
                                                    ,&var[3]
                                                    ,&var[4]
                                                    ,&var[5]);
    }
    else{
        //Compare template and get datetime info 
        if(!AHC_DCFDT_TemplateCmp( szFilename, pDB->szTemplate))
        {
            printc("template string not matched!!\n");
            return;
        }
        sscanfl( szFilename, pDB->szTemplate,&var[0]
                                            ,&var[1]
                                            ,&var[2]
                                            ,&var[3]
                                            ,&var[4]
                                            ,&var[5]);
    }
    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        printc("AHC_DCFDT_AddFile: Invalid date %s\n", szFilename);
        return;
    }

    // TBD: Put callback function

    //psNode = (PSDATETIMEFILE)GlobalAlloc(GMEM_FIXED,sizeof(SDATETIMEFILE));
    psNode = (PSDATETIMEFILE)AHC_DCFDT_FileNodeAlloc();	

    if(psNode == NULL )
    {
        printc("AHC_DCFDT_AddFile: Fail04\r\n");
        return;
    }

    AHC_DCFDT_Lock(pDB);

    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    psNode->nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );

    psNode->byFileType = byFileType;
    psNode->bCharLock  = bIsCharLock;
    
#if ENABLE_DATE_VALUE_DEBUG
    psNode->uwYear    = var[0];
    psNode->uwMonth   = var[1];
    psNode->uwDay     = var[2];
    psNode->uwHour    = var[3];
    psNode->uwMinute  = var[4];
    psNode->uwSecond  = var[5];

#endif //ENABLE_DATE_VALUE_DEBUG

    ulSizeAligned = ALIGN_UP(ulSize, pDB->ulClusterSize);
    
    // Insert
    if( pDB->pFileHead == NULL )
    {
        // First
        pDB->pFileHead = psNode;

        #if RECORD_FILE_SIZE
        psNode->nSizeInCluster = ulSizeAligned/pDB->ulClusterSize;
        #endif //RECORD_FILE_SIZE

        if( sFileAttrib & AHC_FS_ATTR_READ_ONLY )
        {
            psNode->bReadOnly = 1;
        }

        #if (FS_FORMAT_FREE_ENABLE)
        if( sFileAttrib & AHC_FS_ATTR_HIDDEN)
        {
            psNode->bHidden = 1;
        }
        #endif

    }
    else
    {
        PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
        PSDATETIMEFILE psNodeLast = NULL;
        while( psNodeCurr )
        {
            psNodeLast = psNodeCurr;
            if( psNode->nDateTime < psNodeCurr->nDateTime  )
            {
                // Insert before
                psNode->pNext     = psNodeCurr;
                psNode->pPrev     = psNodeCurr->pPrev;

                if( psNodeCurr->pPrev == NULL ) {
                    // Insert at head
                    pDB->pFileHead = psNode;
                }
                else {
                    psNodeCurr->pPrev->pNext = psNode;
                }
                
                psNodeCurr->pPrev = psNode;

                break;
            }
            if( psNode->nDateTime == psNodeCurr->nDateTime )
            {
                // free psNode memory
                AHC_DCFDT_FileNodeFree((void*)psNode);
                printc("Repeated DateTime!!\r\n");
                return;
            }
            psNodeCurr = psNodeCurr->pNext;
        }

        if( psNodeCurr == NULL )
        {
            // Add at tail
            psNode->pPrev     = psNodeLast;
            psNode->pNext     = NULL;
            if( pDB->pFileHead == NULL ) {
                pDB->pFileHead = psNode;
            }
            else {
                psNodeLast->pNext = psNode;
            }
        }

        #if RECORD_FILE_SIZE
        psNode->nSizeInCluster = ulSizeAligned/pDB->ulClusterSize;
        #endif //RECORD_FILE_SIZE

        if( sFileAttrib & AHC_FS_ATTR_READ_ONLY )
        {
            psNode->bReadOnly = 1;
        }
        
        #if FS_FORMAT_FREE_ENABLE
        if( sFileAttrib & AHC_FS_ATTR_HIDDEN)
        {
            psNode->bHidden = 1;
        }
        #endif
    }

    // Update DB statistics
    #if (FS_FORMAT_FREE_ENABLE)
    if( !( sFileAttrib & AHC_FS_ATTR_HIDDEN ) )
    #endif
    {
        pDB->ulTotalObjectNum++;
        pDB->ullTotalObjectSize += ulSizeAligned;

        if( sFileAttrib & AHC_FS_ATTR_READ_ONLY )
        {
            pDB->ulTotalLockedObjects++;
            pDB->ullTotalLockedObjectSize += ulSizeAligned;
            pDB->ullLockedObjectNum[byFileType]++;
            pDB->ullLockedObjectSize[byFileType]+= ulSizeAligned;

        }
        else
        {
            pDB->ulTotalUnlockedObjects++;
            pDB->ullTotalUnlockedObjectSize += ulSizeAligned;

            pDB->ullUnlockedObjectNum[byFileType]++;
            pDB->ullUnlockedObjectSize[byFileType]+= ulSizeAligned;
        }
    }

    //Update LockFileTable
    if(bIsCharLock)
    {
        AIHC_DCFDT_UpdateLockFileTable(psNode, NULL, DCFDT_LOCKTABLE_LOCK);
    }
    
    AHC_DCFDT_Unlock(pDB);

}


void AHC_DCFDT_AddRearFile(PSDATETIMEDCFDB pDB, char* szFilename, UINT32 ulSize, AHC_FS_ATTRIBUTE sFileAttrib )
{
    int var[6];
    DATE_TIME nDateTime;
    UINT8          byFileType;
    ULONG          ulSizeAligned;
    ULONG          ulLen;
    AHC_BOOL       bIsCharLock = AHC_FALSE;
    ulLen = STRLEN(szFilename);

    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        printc("AHC_DCFDT_AddFile: Unknow Type %s\n", szFilename );
        return;
    }
    
    //Compare CHARLOCK template
    if(AHC_DCFDT_TemplateCmp( szFilename, DCFDT_CHARLOCK_REAR_TEMPLATE))
    {
        bIsCharLock = AHC_TRUE;
        //printc("find char lock file %s\r\n", szFilename);
        sscanfl( szFilename, DCFDT_CHARLOCK_TEMPLATE,&var[0]
                                                    ,&var[1]
                                                    ,&var[2]
                                                    ,&var[3]
                                                    ,&var[4]
                                                    ,&var[5]);
    }
    else{
        //Compare template and get datetime info 
        if(!AHC_DCFDT_TemplateCmp( szFilename, pDB->szTemplate_2nd))
        {
            printc("template string not matched!!\n");
            return;
        }
        sscanfl( szFilename, pDB->szTemplate_2nd,&var[0]
                                            ,&var[1]
                                            ,&var[2]
                                            ,&var[3]
                                            ,&var[4]
                                            ,&var[5]);
    }

    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        printc("AHC_DCFDT_AddFile: Invalid date %s\n", szFilename);
        return;
    }

    AHC_DCFDT_Lock(pDB);

    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );
    ulSizeAligned = ALIGN_UP(ulSize, pDB->ulClusterSize);
    // Update DB statistics
    pDB->ullTotalObjectSize += ulSizeAligned;
    
    if(pDB->pFileHead == NULL)
    {
        printc("No File Nodes!!\r\n");
    }
    else
    {
        PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
        while(psNodeCurr)
        {
            if((psNodeCurr->nDateTime == nDateTime) && (psNodeCurr->byFileType == byFileType))
            {
                #if RECORD_FILE_SIZE
                psNodeCurr->nSizeInCluster += ulSizeAligned/pDB->ulClusterSize;
                #endif //RECORD_FILE_SIZE
                psNodeCurr->bIsRearFileExist = AHC_TRUE;
                if(bIsCharLock == AHC_TRUE && psNodeCurr->bCharLock == AHC_FALSE)
                {
                    printc("Front/Rear Cam File Char Lock are not matched\r\n");
                }
                pDB->ulTotalRearCamObjectNum++;
                //update size
                if( sFileAttrib & AHC_FS_ATTR_READ_ONLY )
                {
                    pDB->ullTotalLockedObjectSize += ulSizeAligned;
                    pDB->ullLockedObjectSize[byFileType]+= ulSizeAligned;
                }
                else
                {
                    pDB->ullTotalUnlockedObjectSize += ulSizeAligned;
                    pDB->ullUnlockedObjectSize[byFileType]+= ulSizeAligned;
                }
                
                break;
            }
            psNodeCurr = psNodeCurr->pNext;
        }
    }

    AHC_DCFDT_Unlock(pDB);

}

// Create name without extention part
AHC_BOOL AHC_DCFDT_GetBaseName(PSDATETIMEDCFDB pDB, AHC_RTC_TIME* psRtcTime,char* szNameBuff, UINT uiBuffLength )
{
//    BYTE byFileType;
    INT8 szTemplate[DCF_MAX_FILE_NAME_SIZE];
    MEMSET( szNameBuff, 0x00, uiBuffLength );
    if( !AHC_DT_IsDateValid( psRtcTime->uwYear, psRtcTime->uwMonth, psRtcTime->uwDay ) ||
        !AHC_DT_IsTimeValid( psRtcTime->uwHour, psRtcTime->uwMinute, psRtcTime->uwSecond ) )
    {
        // Date or time is invalid.
        return AHC_FALSE;
    }
    
    if(m_bSetRearCam == AHC_FALSE)
    {
        MEMCPY(szTemplate, pDB->szTemplate, sizeof(pDB->szTemplate));
    }
    else
    {
        MEMCPY(szTemplate, pDB->szTemplate_2nd, sizeof(pDB->szTemplate_2nd));
    }
    sprintc( szNameBuff, szTemplate, psRtcTime->uwYear - DCFDT_FILENAME_YEAR_OFFSET
                                   , psRtcTime->uwMonth
                                   , psRtcTime->uwDay
                                   , psRtcTime->uwHour
                                   , psRtcTime->uwMinute
                                   , psRtcTime->uwSecond );

    /*
    STRCAT( szNameBuff, "." );
    // Ext
    STRCAT( szNameBuff, m_DcfSupportFileType[byFileType] );
    */

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetFullPath(PSDATETIMEDCFDB pDB, AHC_RTC_TIME* psRtcTime, char* szNameBuff, char* FileName)
{
    //BYTE byFileType;
    char* pTemp;
    INT8 szTemplate[DCF_MAX_FILE_NAME_SIZE];
    if( !AHC_DT_IsDateValid( psRtcTime->uwYear - DCFDT_FILENAME_YEAR_OFFSET, psRtcTime->uwMonth, psRtcTime->uwDay ) ||
        !AHC_DT_IsTimeValid( psRtcTime->uwHour, psRtcTime->uwMinute, psRtcTime->uwSecond ) )
    {
        // Date or time is invalid.
        printc("AHC_DCFDT_GetFullPath: Date Time Valid\r\n");
        return AHC_FALSE;
    }
    if(m_bSetRearCam == AHC_TRUE)
    {
        STRCPY( szNameBuff, pDB->szFolderName_2nd );
    }
    else
    {
        STRCPY( szNameBuff, pDB->szFolderName );
    }
    STRCAT( szNameBuff, "\\" ); 

    pTemp = szNameBuff+strlen(szNameBuff); //Rogers: Modify 
    if(m_bSetRearCam == AHC_FALSE)
    {
        MEMCPY(szTemplate, pDB->szTemplate, sizeof(pDB->szTemplate));
    }
    else
    {
        MEMCPY(szTemplate, pDB->szTemplate_2nd, sizeof(pDB->szTemplate_2nd));
    }
    sprintc( pTemp, szTemplate, psRtcTime->uwYear - DCFDT_FILENAME_YEAR_OFFSET
                              , psRtcTime->uwMonth
                              , psRtcTime->uwDay
                              , psRtcTime->uwHour
                              , psRtcTime->uwMinute
                              , psRtcTime->uwSecond );

    if(FileName != NULL)
    {
        STRCPY( FileName, pTemp );
    /*
    sprintc( FileName, pDB->szTemplate, psRtcTime->uwYear - DCFDT_FILENAME_YEAR_OFFSET
                                        , psRtcTime->uwMonth
                                        , psRtcTime->uwDay
                                        , psRtcTime->uwHour
                                        , psRtcTime->uwMinute
                                        , psRtcTime->uwSecond );
    */                                        
    }
    /*   
    STRCAT( pTemp, "." );
    // Ext
    byFileType = pNode->byFileType;   
    STRCAT( pTemp, m_DcfSupportFileType[byFileType] );
    */
    //printc("AHC_DCFDT_GetFullPath: %s\r\n", szNameBuff);

    return AHC_TRUE;
}


AHC_BOOL AHC_DCFDT_DbPreAddFile(PSDATETIMEDCFDB pDB, char* szFilename, AHC_BOOL bDelDuplicated )
{
    int var[6];
    PSDATETIMEFILE psNode;
    BYTE           byFileType;

    ULONG          ulLen;
    AHC_BOOL       bIsCharLock = AHC_FALSE;
    ulLen = STRLEN(szFilename);

    if( pDB->ulFilenameLen != 0    &&
        ulLen != pDB->ulFilenameLen )
    {
#if (DCFDT_DEBUG)    
        printc("AHC_DCFDT_DbPreAddFile: file len error %s\n", szFilename );
#endif
        return AHC_FALSE;
    }


    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        return AHC_FALSE;
    }

    //Compare CHARLOCK template
    if(AHC_DCFDT_TemplateCmp( szFilename, DCFDT_CHARLOCK_TEMPLATE))
    {
        bIsCharLock = AHC_TRUE;
        //printc("find char lock file %s\r\n", szFilename);
        sscanfl( szFilename, DCFDT_CHARLOCK_TEMPLATE,&var[0]
                                                    ,&var[1]
                                                    ,&var[2]
                                                    ,&var[3]
                                                    ,&var[4]
                                                    ,&var[5]);
    }
    else{
        //Compare template and get datetime info 
        if(!AHC_DCFDT_TemplateCmp( szFilename, pDB->szTemplate))
        {
            printc("template string not matched!!\n");
            return;
        }
        sscanfl( szFilename, pDB->szTemplate,&var[0]
                                            ,&var[1]
                                            ,&var[2]
                                            ,&var[3]
                                            ,&var[4]
                                            ,&var[5]);
    }

    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        return AHC_FALSE;
    }

    //-------------------------
    // get filename 屬性

    // Lock Pool
        // Alloc File Unit
    // Unlock pool

    //psNode = (PSDATETIMEFILE)GlobalAlloc(GMEM_FIXED,sizeof(SDATETIMEFILE));
    psNode = (PSDATETIMEFILE)AHC_DCFDT_FileNodeAlloc();

    if(psNode == NULL )
    {
        return AHC_FALSE;
    }

    AHC_DCFDT_Lock(pDB);

    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    psNode->nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );

    psNode->byFileType = byFileType;
    psNode->bCharLock  = bIsCharLock;
    if( m_bCreateRearCamFile == AHC_TRUE )
    {
        psNode->bIsRearFileExist = AHC_TRUE;
        pDB->ulTotalRearCamObjectNum++;
        m_bCreateRearCamFile = AHC_FALSE;
    }
#if ENABLE_DATE_VALUE_DEBUG
    psNode->uwYear    = var[0];
    psNode->uwMonth   = var[1];
    psNode->uwDay     = var[2];
    psNode->uwHour    = var[3];
    psNode->uwMinute  = var[4];
    psNode->uwSecond  = var[5];

#endif //ENABLE_DATE_VALUE_DEBUG

    // Update DB statistics
    pDB->ulTotalObjectNum++;

    // PreAdd only update object number


    // Insert
    if( pDB->pFileHead == NULL )
    {
        // First
        pDB->pFileHead = psNode;
        
        #if RECORD_FILE_SIZE
        psNode->nSizeInCluster = 0;
        #endif //RECORD_FILE_SIZE
    }
    else
    {
        PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
        PSDATETIMEFILE psNodeLast = NULL;
        while( psNodeCurr )
        {
            psNodeLast = psNodeCurr;

            if( (psNode->nDateTime == psNodeCurr->nDateTime) &&
                bDelDuplicated )
            {
                #if DCFDT_DELETE_OLD_DUPLICATE


                PSDATETIMEFILE psNodeLastNew;

                psNodeLastNew = psNodeCurr->pPrev;
                psNodeCurr = psNodeCurr->pNext;
                AHC_DCFDT_DbRemoveFileByNode(pDB, psNodeLast, AHC_TRUE);

                #if 0
                if( psNodeLastNew != NULL )
                {
                    if( psNodeLastNew->pNext != psNodeCurr )
                    {
                        printc("ERROR !!psNodeLastNew->pNext != psNodeCurr \n");
                    }

                }

                if( psNodeCurr != NULL )
                {
                    if( psNodeCurr->pPrev != psNodeLastNew )
                    {
                        printc("ERROR !!psNodeCurr->pPrev != psNodeLastNew \n");
                    }
                    
                }
                #endif

                psNodeLast = psNodeLastNew;

                continue;                    
                #endif //DCFDT_DELETE_OLD_DUPLICATE
            }
            else if( psNode->nDateTime < psNodeCurr->nDateTime  )
            {
                // Insert before
                psNode->pNext     = psNodeCurr;
                psNode->pPrev     = psNodeCurr->pPrev;
                if( psNodeCurr->pPrev == NULL ) {
                    // Insert at head
                    pDB->pFileHead = psNode;
                }
                else{
                    psNodeCurr->pPrev->pNext = psNode;
                }
                psNodeCurr->pPrev = psNode;
                break;
            }
            psNodeCurr = psNodeCurr->pNext;
        }

        if( psNodeCurr == NULL )
        {
            // Add at tail
            psNode->pPrev     = psNodeLast;
            psNode->pNext     = NULL;
            if( pDB->pFileHead == NULL ||
                psNodeLast     == NULL    )
            {
                pDB->pFileHead = psNode;
            }
            else
            {
                psNodeLast->pNext = psNode;
            }
        }

        #if RECORD_FILE_SIZE
        psNode->nSizeInCluster = 0;
        #endif //RECORD_FILE_SIZE

    }
    //Update LockFileTable
    if(bIsCharLock)
    {
        AIHC_DCFDT_UpdateLockFileTable(psNode, NULL, DCFDT_LOCKTABLE_LOCK);
    }
    
    AHC_DCFDT_Unlock(pDB);

    return AHC_TRUE;
}

/*
    This function updates the info of the files in DB, but not be added by post add.

*/
AHC_BOOL AHC_DCFDT_DbUpdateLastFiles(PSDATETIMEDCFDB pDB)
{
    BYTE            byFileType;
    AHC_BOOL        bReturn = AHC_TRUE;

    PSDATETIMEFILE psNodeCurr;
    PSDATETIMEFILE psNodeLast;

    if( pDB == NULL )
    {
        return AHC_FALSE;
    }

    AHC_DCFDT_Lock(pDB);

    psNodeCurr = pDB->pFileHead;

    while( psNodeCurr )
    {
         psNodeLast = psNodeCurr;
        if( psNodeCurr->nSizeInCluster == 0  )
        {
            UINT                ulSizeAligned;
            char                szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
            AHC_FS_ATTRIBUTE    sFileAttrib;
            AHC_FS_FILETIME     sFileTime;

            if( !AHC_DCFDT_GetFullPathByNode(pDB, psNodeCurr, szFullPath, DCF_MAX_FILE_NAME_SIZE ) )
            {
                bReturn = AHC_FALSE;    
                psNodeCurr = psNodeCurr->pNext;                
                continue;            
            }

            if( AHC_ERR_NONE != AHC_FS_FileDirGetInfo(szFullPath, 
                                                      STRLEN(szFullPath),
                                                      &sFileAttrib, 
                                                      &sFileTime, 
                                                      &ulSizeAligned)     )
            {
                psNodeCurr = psNodeCurr->pNext;                
                bReturn = AHC_FALSE;            
                continue;
            }
        
            byFileType = AHC_DCFDT_GetFileType(szFullPath);
        
            if( byFileType == DCFDT_UNKNOW_TYPE )
            {
                // Unknown type
                bReturn = AHC_FALSE;
                psNodeCurr = psNodeCurr->pNext;                
                continue;
            }
        
            // TBD: Update Node value statistics
            ulSizeAligned = ALIGN_UP( ulSizeAligned, pDB->ulClusterSize );
            psNodeCurr->nSizeInCluster = ulSizeAligned/ pDB->ulClusterSize;
            psNodeCurr->bReadOnly      = 0;

            if( sFileAttrib & AHC_FS_ATTR_READ_ONLY )
            {
                psNodeCurr->bReadOnly      = 1;
            }
            // total objects number is added at AHC_DCFDT_DbPreAddFile 
            pDB->ullTotalObjectSize += ulSizeAligned;
            if( psNodeCurr->bReadOnly )
            {
                pDB->ulTotalLockedObjects++;
                pDB->ullTotalLockedObjectSize += ulSizeAligned;
                pDB->ullLockedObjectNum[byFileType]++;
                pDB->ullLockedObjectSize[byFileType]+= ulSizeAligned;
            }
            else
            {
                pDB->ulTotalUnlockedObjects++;
                pDB->ullTotalUnlockedObjectSize += ulSizeAligned;
                pDB->ullUnlockedObjectNum[byFileType]++;
                pDB->ullUnlockedObjectSize[byFileType]+= ulSizeAligned;

            }
            
        }
        psNodeCurr = psNodeCurr->pNext;
    }

    AHC_DCFDT_Unlock(pDB);

    return bReturn;
}

AHC_BOOL AHC_DCFDT_DbPostAddFile(PSDATETIMEDCFDB pDB, char* szFilename)
{
    int var[6];
    BYTE            byFileType;
    DATE_TIME       nDateTime;
    AHC_BOOL        bReturn = AHC_FALSE;

    UINT            ulSizeAligned;
    char            szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};

    AHC_FS_ATTRIBUTE    sFileAttrib;
    AHC_FS_FILETIME     sFileTime;
    AHC_FS_ATTRIBUTE    sRearFileAttrib;
    AHC_FS_FILETIME     sRearFileTime;
    UINT                ulRearFileSizeAligned;
    
    ULONG           ulLen;
    AHC_BOOL        bSetRearCam;
    ulLen = STRLEN(szFilename);

    if( pDB->ulFilenameLen != 0    &&
        ulLen != pDB->ulFilenameLen )
    {
#if (DCFDT_DEBUG)    
        printc("AHC_DCFDT_DbPostAddFile: file len error %s\n", szFilename );
#endif
        return AHC_FALSE;
    }

    // TBD: read file info
    STRCPY( szFullPath, pDB->szFolderName );
    STRCAT( szFullPath, "\\" );
    STRCAT( szFullPath, szFilename );

    if( AHC_ERR_NONE != AHC_FS_FileDirGetInfo(szFullPath, 
                                              STRLEN(szFullPath),
                                              &sFileAttrib, 
                                              &sFileTime, 
                                              &ulSizeAligned)     )
    {
        printc("PostAddFile get %s file info failed!\n", szFullPath );
        return AHC_FALSE;
    }


    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        return bReturn;
    }

    sscanfl( szFilename, pDB->szTemplate,&var[0]
                                        ,&var[1]
                                        ,&var[2]
                                        ,&var[3]
                                        ,&var[4]
                                        ,&var[5]);
    if(!AHC_DCFDT_TemplateCmp( szFilename, pDB->szTemplate))
    {
        printc("template string not matched!!\n");
        return AHC_FALSE;
    }
    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        return bReturn;
    }

    // year offset
    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );
    //
    AHC_DCFDT_Lock(pDB);
    
    if( pDB->pFileHead == NULL )
    {
        // Empty
        return bReturn;
    }
    else
    {
        PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
        PSDATETIMEFILE psNodeLast;

        while( psNodeCurr )
        {
            psNodeLast = psNodeCurr;
            if( nDateTime == psNodeCurr->nDateTime )
            {
                // Avoiding PostAdd be called twice. 
                if( psNodeCurr->nSizeInCluster != 0 &&
                    psNodeCurr->nSizeInCluster < pDB->ullTotalObjectSize/pDB->ulClusterSize )
                {
                    UINT ulOrgSize = psNodeCurr->nSizeInCluster*pDB->ulClusterSize;

                    pDB->ullTotalObjectSize -= ulOrgSize;

                    if( psNodeCurr->bReadOnly )
                    {
                        pDB->ulTotalLockedObjects--;
                        pDB->ullTotalLockedObjectSize -= ulOrgSize;
                        pDB->ullLockedObjectNum[byFileType]--;
                        pDB->ullLockedObjectSize[byFileType] -= ulOrgSize;
                    }
                    else
                    {
                        pDB->ulTotalUnlockedObjects--;
                        pDB->ullTotalUnlockedObjectSize -= ulOrgSize;
                        pDB->ullUnlockedObjectNum[byFileType]--;
                        pDB->ullUnlockedObjectSize[byFileType]-= ulOrgSize;

                    }
                    
                }
                //check IsRearFileExist and get its FileSize
                if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
                {
                    char    szRearFileName[DCF_MAX_FILE_NAME_SIZE];
                    AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
                    AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
                    MEMSET( szRearFileName, 0, sizeof(szRearFileName) );
                    AIHC_DCFDT_CreateFilename(pDB, nDateTime, byFileType, szRearFileName);
                    AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
                    MEMSET( szFullPath, 0, sizeof(szFullPath));
                    STRCPY( szFullPath, pDB->szFolderName_2nd );
                    STRCAT( szFullPath, "\\" );
                    STRCAT( szFullPath, szRearFileName );
                    if( AHC_ERR_NONE != AHC_FS_FileDirGetInfo(szFullPath, 
                                                              STRLEN(szFullPath),
                                                              &sRearFileAttrib, 
                                                              &sRearFileTime, 
                                                              &ulRearFileSizeAligned))
                    {
                        ulRearFileSizeAligned = 0;
                        psNodeCurr->bIsRearFileExist = AHC_FALSE;
                        printc("1PostAddFile get %s rear file info failed!\n", szFullPath );
                    }
                    else
                    {
                        ulRearFileSizeAligned = ALIGN_UP( ulRearFileSizeAligned, pDB->ulClusterSize );
                    }
                }
                else
                {
                    ulRearFileSizeAligned = 0;
                }
                // TBD: Update Node value statistics
                ulSizeAligned = ALIGN_UP( ulSizeAligned, pDB->ulClusterSize );
                ulSizeAligned += ulRearFileSizeAligned;
                psNodeCurr->nSizeInCluster = ulSizeAligned/ pDB->ulClusterSize;
                psNodeCurr->bReadOnly      = 0;

                if( sFileAttrib & AHC_FS_ATTR_READ_ONLY )
                {
                    psNodeCurr->bReadOnly      = 1;
                }

                // total objects number is added at AHC_DCFDT_DbPreAddFile 
                pDB->ullTotalObjectSize += ulSizeAligned;
                if( psNodeCurr->bReadOnly )
                {
                    pDB->ulTotalLockedObjects++;
                    pDB->ullTotalLockedObjectSize += ulSizeAligned;
                    pDB->ullLockedObjectNum[byFileType]++;
                    pDB->ullLockedObjectSize[byFileType]+= ulSizeAligned;
                }
                else
                {
                    pDB->ulTotalUnlockedObjects++;
                    pDB->ullTotalUnlockedObjectSize += ulSizeAligned;
                    pDB->ullUnlockedObjectNum[byFileType]++;
                    pDB->ullUnlockedObjectSize[byFileType]+= ulSizeAligned;

                }
                bReturn = AHC_TRUE;
                break;
                
            }
            psNodeCurr = psNodeCurr->pNext;
        }


    }

    AHC_DCFDT_Unlock(pDB);
    
    return bReturn;

}

    
AHC_BOOL AHC_DCFDT_DbRemoveFileByNode(PSDATETIMEDCFDB pDB, PSDATETIMEFILE  psNodeCurr, AHC_BOOL bDeleteInFS )
{

    BYTE            byFileType;
    ULONG           ulSizeAligned;
    AHC_BOOL        bReturn = AHC_FALSE;
    char            szFilename[DCF_MAX_FILE_NAME_SIZE];

    if( psNodeCurr == NULL )
    {
        return bReturn;
    }

    if(bDeleteInFS)
    {
        AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szFilename, DCF_MAX_FILE_NAME_SIZE);
    }

    byFileType = psNodeCurr->byFileType;

    {
        // unlink this node 
        PSDATETIMEFILE psNodePrev =psNodeCurr->pPrev;
        PSDATETIMEFILE psNodeNext =psNodeCurr->pNext;

        if(psNodePrev == NULL )
        {
            //Head
            pDB->pFileHead = psNodeNext;
            if( psNodeNext )
            {
                psNodeNext->pPrev = NULL;
            }

        }
        else
        {
            psNodePrev->pNext = psNodeNext;
            if( psNodeNext )
            {
                psNodeNext->pPrev = psNodePrev;
            }
        }

    }
    
    if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
    {
        // remove statistic value
        pDB->ulTotalRearCamObjectNum--;
    }
    // Update DB statistics
    if( pDB->ulTotalObjectNum == 0 )
    {
        printc("pDB->ulTotalObjectNum error\n");
    }

    // remove statistic value
    pDB->ulTotalObjectNum--;

    ulSizeAligned = psNodeCurr->nSizeInCluster*pDB->ulClusterSize;
#if RECORD_FILE_SIZE
    pDB->ullTotalObjectSize -= ulSizeAligned;
#endif //RECORD_FILE_SIZE

    if( psNodeCurr->bReadOnly )
    {
        pDB->ulTotalLockedObjects--;
        pDB->ullTotalLockedObjectSize -= ulSizeAligned;

        pDB->ullLockedObjectNum[byFileType]--;
        pDB->ullLockedObjectSize[byFileType]-= ulSizeAligned;

    }
    else
    {
        pDB->ulTotalUnlockedObjects--;
        pDB->ullTotalUnlockedObjectSize -= ulSizeAligned;

        pDB->ullUnlockedObjectNum[byFileType]--;
        pDB->ullUnlockedObjectSize[byFileType]-= ulSizeAligned;
    }


    if(bDeleteInFS)
    {
        char            szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
        AHC_BOOL        bErr = AHC_FALSE;
        STRCPY( szFullPath, pDB->szFolderName );
        STRCAT( szFullPath, "\\" );
        STRCAT( szFullPath, szFilename );

        printc( "%s = %d\n", szFullPath, psNodeCurr->nSizeInCluster*pDB->ulClusterSize);
        
        if( AHC_FALSE == AIHC_DCFDT_DeleteFileInFS(szFullPath, 
                                                   psNodeCurr->bReadOnly) )
        {
            printc("Remove file %s failed!\n", szFullPath );
              bErr = AHC_TRUE;
        }

        if( pDB->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
        {
            AIHC_DCFDT_DeleteNmeaFileInFS(szFullPath, psNodeCurr->bReadOnly);
        }
        if( bErr == AHC_TRUE)
        {
            return AHC_FALSE;
        }
        
        if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
        {
            int var[6];
            DATE_TIME nDateTime;
            AHC_BOOL bSetRearCam;
            memset( szFullPath, 0, sizeof(szFullPath) );
            STRCPY( szFullPath, pDB->szFolderName_2nd );
            STRCAT( szFullPath, "\\" );
            //make Rear Cam FileName

            AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
            AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
            AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szFilename, DCF_MAX_FILE_NAME_SIZE);

            AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
            //
            STRCAT( szFullPath, szFilename );
            if( AIHC_DCFDT_DeleteFileInFS(szFullPath, psNodeCurr->bReadOnly) == AHC_FALSE )
            {
                printc("Remove file %s failed!\n", szFullPath );
            }
        }
        if( bErr == AHC_TRUE)
        {
            return AHC_FALSE;
        }
        
    }
    
    // free psNodeCurr memory
    AHC_DCFDT_FileNodeFree((void*)psNodeCurr);

    bReturn = AHC_TRUE;

    return bReturn;
}


AHC_BOOL AHC_DCFDT_DbRemoveFileByIndex(PSDATETIMEDCFDB pDB, UINT uiIndex, AHC_BOOL bDeleteInFS )
{

    BYTE            byFileType;
    PSDATETIMEFILE  psNodeCurr;
    ULONG           ulSizeAligned;
    AHC_BOOL        bReturn = AHC_FALSE;
    char            szFilename[DCF_MAX_FILE_NAME_SIZE];
    psNodeCurr = AIHC_DCFDT_GetNodeByIndex(pDB,uiIndex);

    if( psNodeCurr == NULL )
    {
        return bReturn;
    }

    if(bDeleteInFS)
    {
        AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szFilename, DCF_MAX_FILE_NAME_SIZE);
    }

    byFileType = psNodeCurr->byFileType;

    {
        // unlink this node 
        PSDATETIMEFILE psNodePrev =psNodeCurr->pPrev;
        PSDATETIMEFILE psNodeNext =psNodeCurr->pNext;

#if 1 //Rogers: Modify               
        if((psNodePrev == NULL) &&  (psNodeNext == NULL)) 
        { //Only one Node
            pDB->pFileHead = NULL;
        }
        else if(psNodePrev == NULL)
        { //The first Node
            pDB->pFileHead = psNodeNext;
            psNodeNext->pPrev = NULL;
        }
        else if(psNodeNext == NULL)
        { //The last Node
            psNodePrev->pNext = NULL;
        }
        else {//The middle Node
            psNodeNext->pPrev = psNodePrev;
            psNodePrev->pNext = psNodeNext;
        }	
#else

        if(psNodePrev == NULL )
        {
            //Head
            pDB->pFileHead = psNodeNext;
            if( psNodeNext )
            {
                psNodeNext->pPrev = NULL;
            }

        }
        else
        {
            psNodePrev->pNext = psNodeNext;
            if( psNodeNext )
            {
                psNodeNext->pPrev = psNodePrev;
            }
        }


#endif        

    }
    
    if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
    {
        // remove statistic value
        pDB->ulTotalRearCamObjectNum--;
    }
    // Update DB statistics
    if( pDB->ulTotalObjectNum == 0 )
    {
        printc("pDB->ulTotalObjectNum error\n");
    }

    // remove statistic value
    pDB->ulTotalObjectNum--;

    ulSizeAligned = psNodeCurr->nSizeInCluster*pDB->ulClusterSize;
    #if RECORD_FILE_SIZE
    pDB->ullTotalObjectSize -= ulSizeAligned;
    #endif //RECORD_FILE_SIZE

    if( psNodeCurr->bReadOnly )
    {
        pDB->ulTotalLockedObjects--;
        pDB->ullTotalLockedObjectSize -= ulSizeAligned;

        pDB->ullLockedObjectNum[byFileType]--;
        pDB->ullLockedObjectSize[byFileType]-= ulSizeAligned;

    }
    else
    {
        pDB->ulTotalUnlockedObjects--;
        pDB->ullTotalUnlockedObjectSize -= ulSizeAligned;

        pDB->ullUnlockedObjectNum[byFileType]--;
        pDB->ullUnlockedObjectSize[byFileType]-= ulSizeAligned;
    }

    if(bDeleteInFS)
    {
        char            szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
        AHC_BOOL        bErr = AHC_FALSE;
        STRCPY( szFullPath, pDB->szFolderName );
        STRCAT( szFullPath, "\\" );
        STRCAT( szFullPath, szFilename );
        
        if( AHC_FALSE == AIHC_DCFDT_DeleteFileInFS(szFullPath, 
                                                   psNodeCurr->bReadOnly) )
        {
            printc("Remove file %s failed!\n", szFullPath );
            bErr = AHC_TRUE;
        }

        if( pDB->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
        {
            AIHC_DCFDT_DeleteNmeaFileInFS(szFullPath, psNodeCurr->bReadOnly);
        }
        if( bErr == AHC_TRUE)
        {
            return AHC_FALSE;
        }
        
        if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
        {
            int var[6];
            DATE_TIME nDateTime;
            AHC_BOOL bSetRearCam;
            memset( szFullPath, 0, sizeof(szFullPath) );
            STRCPY( szFullPath, pDB->szFolderName_2nd );
            STRCAT( szFullPath, "\\" );
            //make Rear Cam FileName

            AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
            AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
            AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szFilename, DCF_MAX_FILE_NAME_SIZE);

            AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
            //
            STRCAT( szFullPath, szFilename );
            if( AIHC_DCFDT_DeleteFileInFS(szFullPath, psNodeCurr->bReadOnly) == AHC_FALSE )
            {
                printc("Remove file %s failed!\n", szFullPath );
            }
        }
        if( bErr == AHC_TRUE)
        {
            return AHC_FALSE;
        }
        
    }

    // free psNodeCurr memory
    AHC_DCFDT_FileNodeFree((void*)psNodeCurr);

    bReturn = AHC_TRUE;

    return bReturn;
}


AHC_BOOL AHC_DCFDT_ChangeFileAttribute(PSDATETIMEDCFDB pDB, UINT uiIndex, AHC_FS_ATTRIBUTE sFileAttrib )
{
    BYTE            byFileType;
    PSDATETIMEFILE  psNodeCurr;
    ULONG           ulSizeAligned;
    AHC_BOOL        bReturn = AHC_FALSE;

    //TBD: get attribute

    psNodeCurr = AIHC_DCFDT_GetNodeByIndex(pDB,uiIndex);

    if( psNodeCurr == NULL )
    {
        return bReturn;
    }

    byFileType = psNodeCurr->byFileType;

    // Update DB statistics
    if( pDB->ulTotalObjectNum == 0 )
    {
        printc("pDB->ulTotalObjectNum error\n");
    }

    ulSizeAligned = psNodeCurr->nSizeInCluster* pDB->ulClusterSize;

    // if attribute change
    {
        AHC_BOOL bReadOnlyOld = (psNodeCurr->bReadOnly)?AHC_TRUE : AHC_FALSE;
        AHC_BOOL bReadOnlyNew = (sFileAttrib & AHC_FS_ATTR_READ_ONLY)?AHC_TRUE : AHC_FALSE;;        

        if( bReadOnlyNew != bReadOnlyOld )
        {

            // Remove read only attribute
            AHC_FS_ATTRIBUTE attrib;
            char            szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};

            AHC_DCFDT_GetFullPathByNode(pDB, psNodeCurr, szFullPath, DCF_MAX_FILE_NAME_SIZE );

            AHC_FS_FileDirGetAttribute(szFullPath, 
                                       STRLEN(szFullPath), &attrib );

            if( bReadOnlyNew )
            {
                pDB->ulTotalLockedObjects++;
                pDB->ullTotalLockedObjectSize         += ulSizeAligned;
                
                pDB->ullLockedObjectNum[byFileType]++;
                pDB->ullLockedObjectSize[byFileType]  += ulSizeAligned;

                pDB->ulTotalUnlockedObjects--;
                pDB->ullTotalUnlockedObjectSize -= ulSizeAligned;
                
                pDB->ullUnlockedObjectNum[byFileType]--;
                pDB->ullUnlockedObjectSize[byFileType]-= ulSizeAligned;
                attrib |= AHC_FS_ATTR_READ_ONLY;
            }
            else
            {
                pDB->ulTotalLockedObjects--;
                pDB->ullTotalLockedObjectSize         -= ulSizeAligned;
                
                pDB->ullLockedObjectNum[byFileType]--;
                pDB->ullLockedObjectSize[byFileType]  -= ulSizeAligned;

                pDB->ulTotalUnlockedObjects++;
                pDB->ullTotalUnlockedObjectSize += ulSizeAligned;
                
                pDB->ullUnlockedObjectNum[byFileType]++;
                pDB->ullUnlockedObjectSize[byFileType]+= ulSizeAligned;
                
                attrib &= ~(AHC_FS_ATTR_READ_ONLY);
            }

            AHC_FS_FileDirSetAttribute(szFullPath, 
                                       STRLEN(szFullPath), attrib );

            
        }
    }
    bReturn = AHC_TRUE;

    return bReturn;
}


// input file name 
// or input index
AHC_BOOL AHC_DCFDT_DbRemoveFile(PSDATETIMEDCFDB pDB, char* szFilename, AHC_BOOL bDeleteInFS)
{
    int var[6];
    PSDATETIMEFILE  psNode;
    BYTE            byFileType;
    DATE_TIME        nDateTime;
    ULONG           ulSizeAligned;
    AHC_BOOL        bReturn = AHC_FALSE;
    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        return bReturn;
    }
    if(AHC_DCFDT_TemplateCmp( szFilename, DCFDT_CHARLOCK_TEMPLATE))
    {
        sscanfl( szFilename, DCFDT_CHARLOCK_TEMPLATE,&var[0]
                                                    ,&var[1]
                                                    ,&var[2]
                                                    ,&var[3]
                                                    ,&var[4]
                                                    ,&var[5]);
    }
    else
    {
        sscanfl( szFilename, pDB->szTemplate,&var[0]
                                            ,&var[1]
                                            ,&var[2]
                                            ,&var[3]
                                            ,&var[4]
                                            ,&var[5]);
    }
    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        return bReturn;
    }

    // year offset
    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );

    // Insert
    if( pDB->pFileHead == NULL )
    {
        // Empty
        return bReturn;
    }
    else
    {
        PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
        PSDATETIMEFILE psNodeLast;

        while( psNodeCurr )
        {
            psNodeLast = psNodeCurr;
            if( nDateTime  == psNodeCurr->nDateTime&&
                byFileType == psNodeCurr->byFileType )
            {
                // 找到node 
                // check. if ext name is the same
                
                // unlink this node 
                PSDATETIMEFILE psNodePrev =psNodeCurr->pPrev;
                PSDATETIMEFILE psNodeNext =psNodeCurr->pNext;

#if 1 //Rogers: Modify               
                if((psNodePrev == NULL) &&  (psNodeNext == NULL)) 
                { //Only one Node
                    pDB->pFileHead = NULL;
                }
                else if(psNodePrev == NULL)
                { //The first Node
                    pDB->pFileHead = psNodeNext;
                    psNodeNext->pPrev = NULL;
                }
                else if(psNodeNext == NULL)
                { //The last Node
                    psNodePrev->pNext = NULL;
                }
                else {//The middle Node
                    psNodeNext->pPrev = psNodePrev;
                    psNodePrev->pNext = psNodeNext;
                }	
#else
                if(psNodePrev == NULL )
                {
                    //Head
                    pDB->pFileHead = psNodeNext;
                    if( psNodeNext )
                    {
                        psNodeNext->pPrev = NULL;
                    }
                
                }
                else
                {
                    psNodePrev->pNext = psNodeNext;
                    if( psNodeNext )
                    {
                        psNodeNext->pPrev = psNodePrev;
                    }
                }

#endif
                if( bDeleteInFS )
                {
                    char   szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
                    AHC_BOOL        bErr = AHC_FALSE;
                    
                    STRCPY( szFullPath, pDB->szFolderName );
                    STRCAT( szFullPath, "\\" );
                    STRCAT( szFullPath, szFilename );
                    
                    if( AHC_FALSE == AIHC_DCFDT_DeleteFileInFS(szFullPath, 
                                                               psNodeCurr->bReadOnly) )
                    {
                        printc("Remove file %s failed!\n", szFullPath );
                        bErr = AHC_TRUE;
                    }
                    
                    if( pDB->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
                    {
                        AIHC_DCFDT_DeleteNmeaFileInFS(szFullPath, psNodeCurr->bReadOnly);
                    }
                    if( bErr == AHC_TRUE)
                    {
                        return AHC_FALSE;
                    }
                    
                    if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
                    {
                        int var[6];
                        DATE_TIME nDateTime;
                        AHC_BOOL bSetRearCam;
                        memset( szFullPath, 0, sizeof(szFullPath) );
                        STRCPY( szFullPath, pDB->szFolderName_2nd );
                        STRCAT( szFullPath, "\\" );
                        //make Rear Cam FileName
                       
                        MEMSET( szFilename, 0, sizeof(szFilename));
                        AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
                        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
                        AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szFilename, DCF_MAX_FILE_NAME_SIZE);

                        AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
                        //
                        STRCAT( szFullPath, szFilename );
                        if( AIHC_DCFDT_DeleteFileInFS(szFullPath, psNodeCurr->bReadOnly) == AHC_FALSE )
                        {
                            printc("Remove file %s failed!\n", szFullPath );
                        }
                    }
                    if( bErr == AHC_TRUE)
                    {
                        return AHC_FALSE;
                    }
                    
                }
                if(psNodeCurr->bIsRearFileExist == AHC_TRUE)
                {
                    // remove statistic value
                    pDB->ulTotalRearCamObjectNum--;
                }
                // Update DB statistics
                if( pDB->ulTotalObjectNum == 0 )
                {
                    printc("pDB->ulTotalObjectNum error\n");
                }
                
                pDB->ulTotalObjectNum--;

                ulSizeAligned  = psNodeCurr->nSizeInCluster * pDB->ulClusterSize;

                #if RECORD_FILE_SIZE
                pDB->ullTotalObjectSize -= psNodeCurr->nSizeInCluster* pDB->ulClusterSize;
                #endif //RECORD_FILE_SIZE

                if( psNode->bReadOnly )
                {
                    pDB->ulTotalLockedObjects--;
                    pDB->ullTotalLockedObjectSize -= psNodeCurr->nSizeInCluster;

                    pDB->ullLockedObjectNum[byFileType]--;
                    pDB->ullLockedObjectSize[byFileType]-= psNodeCurr->nSizeInCluster;
            
                }
                else
                {
                    pDB->ulTotalUnlockedObjects--;
                    pDB->ullTotalUnlockedObjectSize -= psNodeCurr->nSizeInCluster;
            
                    pDB->ullUnlockedObjectNum[byFileType]--;
                    pDB->ullUnlockedObjectSize[byFileType]-= psNodeCurr->nSizeInCluster;
                }

                // TBD:free psNodeCurr memory
                AHC_DCFDT_FileNodeFree((void*)psNodeCurr);
                
                bReturn = AHC_TRUE;
                break;
                
            }
            else if(nDateTime  < psNodeCurr->nDateTime)
            {
                break;
            }

            psNodeCurr = psNodeCurr->pNext;            
        }


    }

    return bReturn;
}

PSDATETIMEFILE AHC_DCFDT_GetNodeByDateTime(PSDATETIMEDCFDB pDB, DATE_TIME nDateTime )
{

    PSDATETIMEFILE  psNode = NULL;

    if( pDB->pFileHead == NULL )
    {
        // Empty
        return psNode;
    }
    else
    {
        PSDATETIMEFILE psNodeCurr;

        psNodeCurr = pDB->pFileHead;
        while( psNodeCurr )
        {
            if( nDateTime == psNodeCurr->nDateTime )
            {
                //AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szNodeFilename, DCF_MAX_FILE_NAME_SIZE);
                //if( 0== STRCMP( szNodeFilename, szFilename ) )
                {
                    // Found
                    psNode  = psNodeCurr;                    
                }

                break;
                
            }
            psNodeCurr = psNodeCurr->pNext;
        }

    }

    return psNode;
}


PSDATETIMEFILE AHC_DCFDT_GetNodeByFilename(PSDATETIMEDCFDB pDB, char* szFilename )
{
    int var[6];
    BYTE            byFileType;
    DATE_TIME       nDateTime;
    PSDATETIMEFILE  psNode = NULL;
    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        return psNode;
    }
    if(AHC_DCFDT_TemplateCmp( szFilename, DCFDT_CHARLOCK_TEMPLATE))
    {
        sscanfl( szFilename, DCFDT_CHARLOCK_TEMPLATE,&var[0]
                                                    ,&var[1]
                                                    ,&var[2]
                                                    ,&var[3]
                                                    ,&var[4]
                                                    ,&var[5]);
    }
    else
    {
        sscanfl( szFilename, pDB->szTemplate,&var[0]
                                            ,&var[1]
                                            ,&var[2]
                                            ,&var[3]
                                            ,&var[4]
                                            ,&var[5]);
    }
    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        return psNode;
    }

    // year offset
    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );

    // Insert
    if( pDB->pFileHead == NULL )
    {
        // Empty
        return psNode;
    }
    else
    {
        PSDATETIMEFILE psNodeCurr;
        char           szNodeFilename[DCF_MAX_FILE_NAME_SIZE];

        psNodeCurr = pDB->pFileHead;
        while( psNodeCurr )
        {
            if( nDateTime == psNodeCurr->nDateTime )
            {
                AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szNodeFilename, DCF_MAX_FILE_NAME_SIZE);
                if( 0== STRCMP( szNodeFilename, szFilename ) )
                {
                    // Found
                    psNode  = psNodeCurr;                    
                }

                break;
                
            }
            psNodeCurr = psNodeCurr->pNext;
        }

    }

    return psNode;
}


UINT32 AHC_DCFDT_DbGetIndexByFilename(PSDATETIMEDCFDB pDB, char* szFilename )
{
    int var[6];
    BYTE            byFileType;
    DATE_TIME       nDateTime;
//    PSDATETIMEFILE  psNode = NULL;
    UINT32          nIdx = -1;
    INT             i;
    byFileType = AHC_DCFDT_GetFileType(szFilename);

    if( byFileType == DCFDT_UNKNOW_TYPE )
    {
        // Unknown type
        return nIdx;
    }
    if(AHC_DCFDT_TemplateCmp( szFilename, DCFDT_CHARLOCK_TEMPLATE))
    {
        sscanfl( szFilename, DCFDT_CHARLOCK_TEMPLATE,&var[0]
                                                    ,&var[1]
                                                    ,&var[2]
                                                    ,&var[3]
                                                    ,&var[4]
                                                    ,&var[5]);
    }
    else
    {
        sscanfl( szFilename, pDB->szTemplate,&var[0]
                                            ,&var[1]
                                            ,&var[2]
                                            ,&var[3]
                                            ,&var[4]
                                            ,&var[5]);
    }
    // Check if date time is valid.
    if( !AHC_DT_IsDateValid( var[0], var[1], var[2] ) ||
        !AHC_DT_IsTimeValid( var[3], var[4], var[5] )    )
    {
        return nIdx;
    }

    // year offset
    var[0] += DCFDT_FILENAME_YEAR_OFFSET;

    // Fill data
    nDateTime = AHC_DT_GetSecondsOffset( var[0], var[1], var[2], var[3], var[4], var[5] );

    // Insert
    if( pDB->pFileHead == NULL )
    {
        // Empty
        return nIdx;
    }
    else
    {
        PSDATETIMEFILE psNodeCurr;
        char           szNodeFilename[DCF_MAX_FILE_NAME_SIZE];

        i=0;
        psNodeCurr = pDB->pFileHead;
        while( psNodeCurr )
        {
            if( nDateTime == psNodeCurr->nDateTime )
            {
                AHC_DCFDT_GetNameByNode(pDB, psNodeCurr, szNodeFilename, DCF_MAX_FILE_NAME_SIZE);
                if( 0== STRCMP( szNodeFilename, szFilename ) )
                {
                    // Found
                    nIdx  = i;                    
                }

                break;
                
            }
            psNodeCurr = psNodeCurr->pNext;
            i++;
        }

    }

    return nIdx;
}

PSDATETIMEFILE AIHC_DCFDT_GetNodeByIndex(PSDATETIMEDCFDB pDB, UINT uiIndex )
{
    
    PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
    int            i = 0;

    // Check index range
    if( uiIndex >= pDB->ulTotalObjectNum )
    {
        return NULL;
    }
  
    while( psNodeCurr && i <= uiIndex  )
    {
        #if (FS_FORMAT_FREE_ENABLE)
        if(psNodeCurr->bHidden)
        {
            psNodeCurr = psNodeCurr->pNext;
            continue;
        }
        #endif
        
        if( uiIndex == i )
        {
            // The node found.
            return psNodeCurr;
        }
        i++;
        psNodeCurr = psNodeCurr->pNext;
    }

    return NULL;
}


AHC_BOOL AHC_DCFDT_GetNameByNode(PSDATETIMEDCFDB pDB, PSDATETIMEFILE pNode, char* szNameBuff, UINT uiBuffLength )
{
    BYTE byFileType;
    AHC_RTC_TIME sRtcTime={0};

    MEMSET( szNameBuff, 0x00, uiBuffLength );

    if( AHC_DT_SecondsToDateTime(pNode->nDateTime, &sRtcTime ))
    {
        INT8 szTemplate[DCF_MAX_FILE_NAME_SIZE];
        if(m_bSetRearCam == AHC_FALSE)
        {
            if(pNode->bCharLock == AHC_TRUE){
                MEMCPY(szTemplate, DCFDT_CHARLOCK_TEMPLATE, sizeof(DCFDT_CHARLOCK_TEMPLATE));
            }else{
                MEMCPY(szTemplate, pDB->szTemplate, sizeof(pDB->szTemplate));
            }
        }
        else
        {
            if(pNode->bCharLock == AHC_TRUE){
                MEMCPY(szTemplate, DCFDT_CHARLOCK_REAR_TEMPLATE, sizeof(DCFDT_CHARLOCK_REAR_TEMPLATE));
            }else{
                MEMCPY(szTemplate, pDB->szTemplate_2nd, sizeof(pDB->szTemplate_2nd));
            }
        }
        sprintc( szNameBuff, szTemplate, sRtcTime.uwYear - DCFDT_FILENAME_YEAR_OFFSET
                                       , sRtcTime.uwMonth
                                       , sRtcTime.uwDay
                                       , sRtcTime.uwHour
                                       , sRtcTime.uwMinute
                                       , sRtcTime.uwSecond );

        STRCAT( szNameBuff, "." );
        // Ext
        byFileType = pNode->byFileType;    
        STRCAT( szNameBuff, m_DcfSupportFileType[byFileType] );

        return AHC_TRUE;
    }

    return AHC_FALSE;
}

AHC_BOOL AHC_DCFDT_GetFullPathByNode(PSDATETIMEDCFDB pDB, PSDATETIMEFILE pNode, char* szNameBuff, UINT uiBuffLength )
{
    BYTE byFileType;
    AHC_RTC_TIME sRtcTime={0};
    char* pTemp;

    MEMSET( szNameBuff, 0x00, uiBuffLength );

    if(m_bSetRearCam == AHC_TRUE)
    {
        if(pNode->bIsRearFileExist == AHC_FALSE)
        {
            printc("Warning! This node does not exist Rear Cam file %d\r\n");
            return AHC_FALSE;
        }
        STRCPY( szNameBuff, pDB->szFolderName_2nd );
    }
    else
    {
        STRCPY( szNameBuff, pDB->szFolderName );
    }
    STRCAT( szNameBuff, "\\" );    

    if( AHC_DT_SecondsToDateTime(pNode->nDateTime, &sRtcTime ))
    {
        INT8 szTemplate[DCF_MAX_FILE_NAME_SIZE];
        pTemp = szNameBuff+strlen(szNameBuff); //Rogers: Modify
        if(m_bSetRearCam == AHC_FALSE)
        {
            if(pNode->bCharLock == AHC_TRUE){
                MEMCPY(szTemplate, DCFDT_CHARLOCK_TEMPLATE, sizeof(DCFDT_CHARLOCK_TEMPLATE));
            }else{
                MEMCPY(szTemplate, pDB->szTemplate, sizeof(pDB->szTemplate));
            }
        }
        else
        {
            if(pNode->bCharLock == AHC_TRUE){
                MEMCPY(szTemplate, DCFDT_CHARLOCK_REAR_TEMPLATE, sizeof(DCFDT_CHARLOCK_REAR_TEMPLATE));
            }else{
                MEMCPY(szTemplate, pDB->szTemplate_2nd, sizeof(pDB->szTemplate_2nd));
            }
        }
        sprintc( pTemp, szTemplate, sRtcTime.uwYear - DCFDT_FILENAME_YEAR_OFFSET
                                  , sRtcTime.uwMonth
                                  , sRtcTime.uwDay
                                  , sRtcTime.uwHour
                                  , sRtcTime.uwMinute
                                  , sRtcTime.uwSecond );

        STRCAT( pTemp, "." );
        // Ext
        byFileType = pNode->byFileType;    
        STRCAT( pTemp, m_DcfSupportFileType[byFileType] );

        return AHC_TRUE;
    }

    return AHC_FALSE;
}


AHC_BOOL AHC_DCFDT_GetNameByIndex( PSDATETIMEDCFDB pDB, UINT32 uiIndex, char* szNameBuff, UINT uiBuffLength )
{
    PSDATETIMEFILE pNode;

    // Check index range
    if( uiIndex >= pDB->ulTotalObjectNum )
    {
        MEMSET( szNameBuff, 0x00, uiBuffLength );
        return AHC_FALSE;
    }

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );
    
    if( pNode != 0 )
    {
        if( AHC_DCFDT_GetNameByNode(pDB, pNode, szNameBuff, uiBuffLength ) )
        {
            return AHC_TRUE;
        }
    }
    return AHC_FALSE;
}

AHC_BOOL AHC_DCFDT_GetFullPathByIndex( PSDATETIMEDCFDB pDB, UINT32 uiIndex, char* szNameBuff, UINT uiBuffLength )
{
    PSDATETIMEFILE pNode;

    // Check index range
    if( uiIndex >= pDB->ulTotalObjectNum )
    {
        MEMSET( szNameBuff, 0x00, uiBuffLength );
        return AHC_FALSE;
    }

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );
    
    if( pNode != 0 )
    {
        if( AHC_DCFDT_GetFullPathByNode(pDB, pNode, szNameBuff, uiBuffLength ) )
        {
            return AHC_TRUE;
        }
    }

    return AHC_FALSE;
}


AHC_BOOL AIHC_DCFDT_GetInfo(PSDATETIMEDCFDB pDB, UINT32 uiIndex, DCF_INFO *Info)
{
    PSDATETIMEFILE pNode;


    if( uiIndex >= pDB->ulTotalObjectNum )
    {
        MEMSET( Info, 0x00, sizeof(Info) );
        return AHC_FALSE;
    }

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );

    if( pNode != 0 )
    {
        AHC_RTC_TIME sRtcTime={0};
        AHC_DT_SecondsToDateTime(pNode->nDateTime, &sRtcTime );

        Info->uiFileSize  = pNode->nSizeInCluster * pDB->ulClusterSize ;
        Info->FileType    = pNode->byFileType;
        Info->bReadOnly   = pNode->bReadOnly;
        Info->bCharLock   = pNode->bCharLock;

        Info->uwYear      = sRtcTime.uwYear;
        Info->uwMonth     = sRtcTime.uwMonth;
        Info->uwDay       = sRtcTime.uwDay;
        Info->uwHour      = sRtcTime.uwHour;
        Info->uwMinute    = sRtcTime.uwMinute;
        Info->uwSecond    = sRtcTime.uwSecond;

        AHC_DCFDT_GetNameByNode(pDB, pNode, Info->FileName, MAX_FILE_NAME_SIZE ) ;

        #if DCFDT_DEBUG
        {
            int nNameLength = STRLEN(Info->FileName);
            if( nNameLength < 20 ||
                nNameLength > 30 )
            {
                printc("[WARN] Name length might be abnormal !\n");
            }

            printc( "File name:%s\n", Info->FileName );
        }

        if( !AHC_DT_IsDateValid( sRtcTime.uwYear, sRtcTime.uwMonth, sRtcTime.uwDay ) ||
            !AHC_DT_IsTimeValid( sRtcTime.uwHour, sRtcTime.uwMinute, sRtcTime.uwSecond )    )
        {
            printc("[ERR] Datetime is incorrect!\n");
        }


        if( pNode->uwYear      != sRtcTime.uwYear  ||
            pNode->uwMonth     != sRtcTime.uwMonth  ||
            pNode->uwDay       != sRtcTime.uwDay  ||
            pNode->uwHour      != sRtcTime.uwHour  ||
            pNode->uwMinute    != sRtcTime.uwMinute  ||
            pNode->uwSecond    != sRtcTime.uwSecond     )
        {
            printc("The date time is incorrect! %d");
        }

        #endif //DCFDT_DEBUG
       
    }

    return AHC_TRUE;
}

UINT32 AHC_DCFDT_GetTotalCluster(void)
{
    return  m_sMediaInfo.ulTotalClusters;
}

AHC_BOOL AHC_DCFDT_LockInit(PSDATETIMEDCFDB pDB)
{
    pDB->smLock = AHC_OS_CreateSem(1);
    if( pDB->smLock == 0xFF )
    {
        return AHC_FALSE;
    }

    return AHC_TRUE;

}

AHC_BOOL AHC_DCFDT_LockUninit(PSDATETIMEDCFDB pDB)
{
    if( AHC_OS_DeleteSem(pDB->smLock) == 0xFF )
    {
        return AHC_FALSE;
    }
    return AHC_TRUE;
}


AHC_BOOL AHC_DCFDT_LockEx(PSDATETIMEDCFDB pDB, UINT32 uiTimeOutTick )
{
    if (AHC_OS_AcquireSem(pDB->smLock, uiTimeOutTick) != OS_NO_ERR) 
    {
        return AHC_FALSE;
    }

    return AHC_TRUE;

}

AHC_BOOL AHC_DCFDT_Lock(PSDATETIMEDCFDB pDB )
{
    // Wait forever
    if (AHC_OS_AcquireSem(pDB->smLock, 0) != OS_NO_ERR) 
    {
        return AHC_FALSE;
    }

    return AHC_TRUE;

}

AHC_BOOL AHC_DCFDT_Unlock(PSDATETIMEDCFDB pDB)
{
    if (AHC_OS_ReleaseSem(pDB->smLock) != OS_NO_ERR) 
    {
        return AHC_FALSE;
    }

    return AHC_TRUE;

}

// TBD: Pool Lock

UINT32 AHC_DCFDT_DbGetReadOnlyCluster(PSDATETIMEDCFDB pDB)
{
    return pDB->ullTotalLockedObjectSize/pDB->ulClusterSize;
}

UINT32 AHC_DCFDT_DbGetCyclicCluster(PSDATETIMEDCFDB pDB)
{
    // The size can be cyclic usage.
    return pDB->ullTotalUnlockedObjectSize/pDB->ulClusterSize;
}

AHC_BOOL AIHC_DCFDT_DbDeleteCyclicFilebySize(PSDATETIMEDCFDB pDB, UINT64 ullSize )
{
    
    UINT64 ullSizeDeleted = 0;
    UINT32 uiSize;
    PSDATETIMEFILE pNode;
    UINT idx = 0;

    while( (ullSizeDeleted < ullSize)  &&
           (pDB->pFileHead != NULL)       )
    {
        //Remove Head
        pNode = pDB->pFileHead;
        while(pNode->bReadOnly || pNode->bCharLock){
        	if(!pNode->pNext)
          {
                idx = 0;
                pNode = pDB->pFileHead;
    	      break;
          }
        	idx++;
        	pNode = pNode->pNext;
        }
        uiSize = pNode->nSizeInCluster * pDB->ulClusterSize;
        AHC_DCFDT_DbRemoveFileByIndex(pDB, idx, AHC_TRUE );

        ullSizeDeleted += uiSize;
        idx = 0;
    }

	if(ullSizeDeleted >= ullSize)
		return AHC_TRUE;
	else
		return AHC_FALSE;
}

void AIHC_DCFDT_DbDeleteCyclicFilebyNum(PSDATETIMEDCFDB pDB, UINT32 ulNum )
{
    
    
    while(ulNum--)
    {
        if ( (ulNum < pDB->ulTotalObjectNum) &&
             (pDB->pFileHead != NULL) &&
             (pDB->ulTotalObjectNum > 0) ) {
            AHC_DCFDT_DbRemoveFileByIndex(pDB, 0, AHC_TRUE);
        }
    }
/*
    while( (ulNumDeleted <= ulNum)  &&
           (pDB->pFileHead != NULL) &&
           (pDB->ulTotalObjectNum > 0) )
    {
        printc("NumDeleted=%d, ulNum=%d\n", ulNumDeleted, ulNum);
        AHC_DCFDT_DbRemoveFileByIndex(pDB, 0, AHC_TRUE );
        ulNumDeleted += 1;
    }
*/
}

UINT32 AHC_DCFDT_DbGetFileCluster(PSDATETIMEDCFDB pDB)
{
    UINT32 ulClusters;

    ulClusters  = pDB->ullTotalObjectSize  / pDB->ulClusterSize;
    //ulClusters += pDB->ulPayloadInCluster;

    return ulClusters;
}


UINT32 AHC_DCFDT_DbGetClusterTH(PSDATETIMEDCFDB pDB)
{
    UINT32 ulClusters;
    ulClusters  = pDB->ulMaxSizeThresholdInCluster;

    return ulClusters;
}

AHC_BOOL AHC_DCFDT_GetFile(int iIndex, char* szFilename, int iSize)
{

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetDCFIndex(INT8 *pchDirName, INT8 *pchFileName, UINT32 *puiIndex)
{
    //AHC_GetDCFIndex();

    return AHC_FALSE;
}


//19. 現在認得的副檔名的檔案個數
AHC_BOOL AHC_DCFDT_GetMaxDCFObjCount(UINT32 *puiImgIndex)
{
    //AHC_DCF_GetTotalFileCount(0);
    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_KeepFreeSpace(PSDATETIMEDCFDB pDB, UINT64 ullSize )
{
    // current free space = max size threshold - total object size

    // max free size can get = max size threshold - read only size

    // if free size < ullSize
    //  then delete files

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_DbSetCurrentIndex(PSDATETIMEDCFDB pDB, UINT32 uiImgIndex)
{

    if( uiImgIndex >= pDB->ulTotalObjectNum )
    {
        printc("[ERR] AHC_DCFDT_DbSetCurrentIndex is out of range!\n");
        return AHC_FALSE;
    }

    pDB->uiCurrentIndex = uiImgIndex;
    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_DbGetCurrentIndex(PSDATETIMEDCFDB pDB, UINT32 *puiImgIndex)
{
    if( pDB->uiCurrentIndex >= pDB->ulTotalObjectNum )
    {
        printc("[ERR] AHC_DCFDT_DbGetCurrentIndex is out of range!\n");
        pDB->uiCurrentIndex = 0;
    }

    *puiImgIndex = pDB->uiCurrentIndex;
   
    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_MemPoolFSInit(UINT32 uiMaxMemSize)
{
    AHC_BOOL bInit = AHC_FALSE;
    AHC_BOOL bRet  = AHC_FALSE;

    if( !bInit )
    {
        
        hFileNodeMemPool = AHC_MemPoolFS_InitPool((PBYTE)AHC_DTDCF_MEM_START, uiMaxMemSize, sizeof(SDATETIMEFILE) );

        if( hFileNodeMemPool != NULL )
        {
            bRet = AHC_TRUE;
        }

    }

    return bRet;
}

AHC_BOOL AHC_DCFDT_MemPoolFSUninit(void)
{
    if( hFileNodeMemPool )
    {
        AHC_MemPoolFS_UninitPool(hFileNodeMemPool);
        hFileNodeMemPool = NULL;
        return AHC_TRUE;
    }
    return AHC_FALSE;
}

void* AHC_DCFDT_FileNodeAlloc( void )
{
    return AHC_MemPoolFS_Calloc(hFileNodeMemPool);
}

AHC_BOOL AHC_DCFDT_FileNodeFree( void* pbyBuff )
{
    MEMSET(pbyBuff, 0xFF, hFileNodeMemPool->uiChunkSize);
    return AHC_MemPoolFS_Free(hFileNodeMemPool,pbyBuff);
}

/*
DCF_FILE_READ_ONLY: 設定檔案為唯讀
DCF_FILE_NON_READ_ONLY: 設定檔案為非唯讀
DCF_FILE_COPY: 拷貝檔案
x DCF_FILE_CHAR_LOCK: 鎖定G-sensor lock檔案
x DCF_FILE_CHAR_UNLOCK: 解鎖G-sensor lock檔案
x DCF_FILE_MOVE_SUBDB: 移動檔案到SubDB
DCF_FILE_DELETE: 刪除檔案
DCF_FILE_RENAME: 改檔名
{
    //AHC_DCF_FileOperation
    return AHC_FALSE;
}

*/

AHC_BOOL AHC_DCFDT_FileOperation(char *pPathName, 
                                   char *pFileName, 
                                   UINT8 byOperation, 
                                   char *pDestPath, 
                                   char *pDestFileName)

{
    UINT32              error = MMP_ERR_NONE;
    char                szSrcFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    char                szDstFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    AHC_BOOL            bReadOnlyNew = AHC_FALSE;

    MEMSET(szSrcFullPath , 0, sizeof(szSrcFullPath));
    MEMSET(szDstFullPath, 0, sizeof(szDstFullPath));

    switch(byOperation){
    
    case DCF_FILE_READ_ONLY:
    case DCF_FILE_NON_READ_ONLY:
    {
        PSDATETIMEDCFDB pSrcDb;
        PSDATETIMEFILE  psNode;
        AHC_BOOL bReadOnlyOld;
        bReadOnlyNew = (byOperation == DCF_FILE_READ_ONLY ? AHC_TRUE : AHC_FALSE);

        pSrcDb = AHC_DCFDT_GetDBByFolderName((char*)pPathName);

        if( pSrcDb == NULL )
        {
            return AHC_FALSE;
        }

        psNode = AHC_DCFDT_GetNodeByFilename(pSrcDb, (char*)pFileName );

        if( psNode == NULL )
        {
            return AHC_FALSE;
        }

        bReadOnlyOld = (psNode->bReadOnly)?AHC_TRUE : AHC_FALSE;
        psNode->bReadOnly = (bReadOnlyNew == AHC_TRUE ? 1 : 0); //1;

        if( bReadOnlyNew != bReadOnlyOld )
        {

            // Remove read only attribute
            ULONG            ulSizeAligned;
            BYTE             byFileType;
            AHC_FS_ATTRIBUTE attrib;

            STRCPY( szSrcFullPath, pSrcDb->szFolderName );
            STRCAT( szSrcFullPath, "\\" );
            STRCAT( szSrcFullPath, (char*)pFileName );

            AHC_FS_FileDirGetAttribute(szSrcFullPath, 
                                       STRLEN(szSrcFullPath), &attrib );

            byFileType    = psNode->byFileType;

            ulSizeAligned = psNode->nSizeInCluster * pSrcDb->ulClusterSize;

            if( bReadOnlyNew )
            {
                pSrcDb->ulTotalLockedObjects++;
                pSrcDb->ullTotalLockedObjectSize         += ulSizeAligned;
                
                pSrcDb->ullLockedObjectNum[byFileType]++;
                pSrcDb->ullLockedObjectSize[byFileType]  += ulSizeAligned;

                pSrcDb->ulTotalUnlockedObjects--;
                pSrcDb->ullTotalUnlockedObjectSize -= ulSizeAligned;
                
                pSrcDb->ullUnlockedObjectNum[byFileType]--;
                pSrcDb->ullUnlockedObjectSize[byFileType]-= ulSizeAligned;
                attrib |= AHC_FS_ATTR_READ_ONLY;
            }
            else
            {
                pSrcDb->ulTotalLockedObjects--;
                pSrcDb->ullTotalLockedObjectSize         -= ulSizeAligned;
                
                pSrcDb->ullLockedObjectNum[byFileType]--;
                pSrcDb->ullLockedObjectSize[byFileType]  -= ulSizeAligned;

                pSrcDb->ulTotalUnlockedObjects++;
                pSrcDb->ullTotalUnlockedObjectSize += ulSizeAligned;
                
                pSrcDb->ullUnlockedObjectNum[byFileType]++;
                pSrcDb->ullUnlockedObjectSize[byFileType]+= ulSizeAligned;
                
                attrib &= ~(AHC_FS_ATTR_READ_ONLY);
            }

            AHC_FS_FileDirSetAttribute(szSrcFullPath, 
                                       STRLEN(szSrcFullPath), attrib );

        }

    }
    break;
    
    case DCF_FILE_COPY:
    {
        PSDATETIMEDCFDB pDstDb;

        STRCPY( szSrcFullPath, pPathName );
        STRCAT( szSrcFullPath, "\\" );
        STRCAT( szSrcFullPath, pFileName );
        
        STRCPY( szDstFullPath, pDestPath );
        STRCAT( szDstFullPath, "\\" );
        STRCAT( szDstFullPath, pDestFileName );
        
        error = AHC_FS_FileCopy(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));

        if( error == MMP_ERR_NONE )
        {
            pDstDb = AHC_DCFDT_GetDBByFolderName((char*)pDestPath);
            
            if( pDstDb == NULL )
            {
                return AHC_FALSE;
            }

            if( pDstDb->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
            {
                AIHC_DCFDT_ChangeToNmeaName(szSrcFullPath);
                AIHC_DCFDT_ChangeToNmeaName(szDstFullPath);
                AHC_FS_FileCopy(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));
            }
            
            AHC_DCFDT_DbPreAddFile(pDstDb,pFileName, AHC_FALSE);
            AHC_DCFDT_DbPostAddFile(pDstDb,pFileName);            
        }

    }
    break;
    //New Feature of CHAR_LOCK by Jerry Li
    case DCF_FILE_CHAR_LOCK:
    {        
        PSDATETIMEDCFDB pDB;
        PSDATETIMEFILE  psNode;
        char            szFilepath[DCF_MAX_FILE_NAME_SIZE];
        char            szRearFilepath[DCF_MAX_FILE_NAME_SIZE];
        char            szLockFilepath[DCF_MAX_FILE_NAME_SIZE];
        char            szLockRearFilepath[DCF_MAX_FILE_NAME_SIZE];
        AHC_BOOL        bSetRearCam;
        pDB = AHC_DCFDT_GetDBByFolderName((char*)pPathName);
        if( pDB == NULL )
        {
            return AHC_FALSE;
        }
        psNode = AHC_DCFDT_GetNodeByFilename(pDB, (char*)pFileName );
        if( psNode == NULL )
        {
            return AHC_FALSE;
        }
        //get original filename
        AHC_DCFDT_GetFullPathByNode(pDB, psNode, szFilepath, DCF_MAX_FILE_NAME_SIZE);
        if(psNode->bIsRearFileExist){
            AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
            AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
            AHC_DCFDT_GetFullPathByNode(pDB, psNode, szRearFilepath, DCF_MAX_FILE_NAME_SIZE);
            AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
        }
        //set charlock = true
        if(psNode->bCharLock == AHC_TRUE){
            printc("Warning!! DCF_FILE_CHAR_UNLOCK! File is CharLock\r\n");
        }
        else
        {
            AHC_BOOL bDelete;
            psNode->bCharLock = AHC_TRUE;
            //get charlock filename
            AHC_DCFDT_GetFullPathByNode(pDB, psNode, szLockFilepath, DCF_MAX_FILE_NAME_SIZE);
            if(psNode->bIsRearFileExist){
                AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
                AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
                AHC_DCFDT_GetFullPathByNode(pDB, psNode, szLockRearFilepath, DCF_MAX_FILE_NAME_SIZE);
                AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
            }
            //Rename to char lock name
            error = AHC_FS_FileDirRename(szFilepath, STRLEN(szFilepath), szLockFilepath, STRLEN(szLockFilepath));
            if(error != MMP_ERR_NONE)
            {
                printc("%s failed!! szLockFilepath %s\r\n", __func__, szLockFilepath);
            }
            if(psNode->bIsRearFileExist){
                error = AHC_FS_FileDirRename(szRearFilepath, STRLEN(szRearFilepath), szLockRearFilepath, STRLEN(szLockRearFilepath));
                if(error != MMP_ERR_NONE)
                {
                    printc("%s failed!! szLockFilepath %s\r\n", __func__, szLockRearFilepath);
                }
            }
            AIHC_DCFDT_UpdateLockFileTable(psNode, &bDelete, DCFDT_LOCKTABLE_LOCK);
            *pDestFileName = (char)bDelete;
        }
    }
    break;
    case DCF_FILE_CHAR_UNLOCK:
    {
        PSDATETIMEDCFDB pDB;
        PSDATETIMEFILE  psNode;
        char            szFilepath[DCF_MAX_FILE_NAME_SIZE];
        char            szRearFilepath[DCF_MAX_FILE_NAME_SIZE];
        char            szLockFilepath[DCF_MAX_FILE_NAME_SIZE];
        char            szLockRearFilepath[DCF_MAX_FILE_NAME_SIZE];
        AHC_BOOL        bSetRearCam;
        
        pDB = AHC_DCFDT_GetDBByFolderName((char*)pPathName);
        if( pDB == NULL )
        {
            return AHC_FALSE;
        }
        psNode = AHC_DCFDT_GetNodeByFilename(pDB, (char*)pFileName );
        if( psNode == NULL )
        {
            return AHC_FALSE;
        }
        //get charlock filename
        AHC_DCFDT_GetFullPathByNode(pDB, psNode, szLockFilepath, DCF_MAX_FILE_NAME_SIZE);
        if(psNode->bIsRearFileExist){
            AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
            AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
            AHC_DCFDT_GetFullPathByNode(pDB, psNode, szLockRearFilepath, DCF_MAX_FILE_NAME_SIZE);
            AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
        }
        //set charlock = false
        if(psNode->bCharLock == AHC_FALSE){
            printc("Warning!! DCF_FILE_CHAR_UNLOCK! File is not CharLock\r\n");
        }
        else
        {
            psNode->bCharLock = AHC_FALSE;
            //get original filename
            AHC_DCFDT_GetFullPathByNode(pDB, psNode, szFilepath, DCF_MAX_FILE_NAME_SIZE);
            if(psNode->bIsRearFileExist){
                AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
                AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
                AHC_DCFDT_GetFullPathByNode(pDB, psNode, szRearFilepath, DCF_MAX_FILE_NAME_SIZE);
                AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
            }
            //Rename to original name
            error = AHC_FS_FileDirRename(szLockFilepath, STRLEN(szLockFilepath), szFilepath, STRLEN(szFilepath));
            if(error != MMP_ERR_NONE)
            {
                printc("%s failed!! szFilepath %s\r\n", __func__, szFilepath);
            }
            if(psNode->bIsRearFileExist){
                error = AHC_FS_FileDirRename(szLockRearFilepath, STRLEN(szLockRearFilepath), szRearFilepath, STRLEN(szRearFilepath));
                if(error != MMP_ERR_NONE)
                {
                    printc("%s failed!! szRearFilepath %s\r\n", __func__, szRearFilepath);
                }
            }   
            AIHC_DCFDT_UpdateLockFileTable(psNode, NULL, DCFDT_LOCKTABLE_UNLOCK);
        }
    }
    break;
    
    case DCF_FILE_MOVE_SUBDB:
    {
        PSDATETIMEDCFDB pSrcDb;
        PSDATETIMEFILE  psSrcNode;
        PSDATETIMEDCFDB pDstDb;
        PSDATETIMEFILE  psDstNode;

        
        STRCPY( szSrcFullPath, pPathName );
        STRCAT( szSrcFullPath, "\\" );
        STRCAT( szSrcFullPath, pFileName );
        
        STRCPY( szDstFullPath, pDestPath );
        STRCAT( szDstFullPath, "\\" );
        STRCAT( szDstFullPath, pDestFileName );
        
        pSrcDb = AHC_DCFDT_GetDBByFolderName((char*)pPathName);

        if( pSrcDb == NULL )
        {
            return AHC_FALSE;
        }

        psSrcNode = AHC_DCFDT_GetNodeByFilename(pSrcDb, (char*)pFileName );

        if( psSrcNode == NULL )
        {
            return AHC_FALSE;
        }

        pDstDb = AHC_DCFDT_GetDBByFolderName((char*)pDestPath);
        
        if( pDstDb == NULL )
        {
            return AHC_FALSE;
        }

        psDstNode = AHC_DCFDT_GetNodeByFilename(pDstDb, (char*)pDestFileName );

        if( psDstNode != NULL )
        {
            // Delete target exist file
            AHC_DCFDT_DbRemoveFile(pDstDb, pFileName, AHC_TRUE);
        }

        error = AHC_FS_FileDirMove(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));

        if( pDstDb->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
        {
            AIHC_DCFDT_ChangeToNmeaName(szSrcFullPath);
            AIHC_DCFDT_ChangeToNmeaName(szDstFullPath);
            AHC_FS_FileDirMove(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));
        }

        if( error == MMP_ERR_NONE )
        {
            AHC_DCFDT_DbRemoveFile(pSrcDb, pFileName, AHC_FALSE);
            AHC_DCFDT_DbPreAddFile(pDstDb,pDestFileName, AHC_FALSE);
            AHC_DCFDT_DbPostAddFile(pDstDb,pDestFileName);            
        }
    }
    break;
    
    case DCF_FILE_DELETE:
    {
        PSDATETIMEDCFDB pSrcDb;
        PSDATETIMEFILE  psSrcNode;

        pSrcDb = AHC_DCFDT_GetDBByFolderName((char*)pPathName);

        if( pSrcDb == NULL )
        {
            printc("DCF_FILE_DELETE: Fail0\r\n");
            return AHC_FALSE;
        }

        psSrcNode = AHC_DCFDT_GetNodeByFilename(pSrcDb, (char*)pFileName );

        if( psSrcNode == NULL )
        {
            printc("DCF_FILE_DELETE: Fail1\r\n");
            return AHC_FALSE;
        }
        if( psSrcNode->bReadOnly == AHC_TRUE )
        {
            printc("DCF_FILE_DELETE: This file is Read only!!\r\n");
            return AHC_FALSE;
        }
        if( psSrcNode->bCharLock == AHC_TRUE )
        {
            printc("DCF_FILE_DELETE: This file is Char Lock!!\r\n");
            return AHC_FALSE;
        }
        if( !AHC_DCFDT_DbRemoveFile(pSrcDb, pFileName, AHC_TRUE) )
        {
            printc("DCF_FILE_DELETE: Fail2\r\n");
            return AHC_FALSE;
        }

    }
    break;

//	case DCF_FILE_MOVE:	
    case DCF_FILE_RENAME:
    {
        PSDATETIMEDCFDB pSrcDb;
        PSDATETIMEFILE  psSrcNode;
        PSDATETIMEDCFDB pDstDb;
        PSDATETIMEFILE  psDstNode;

        STRCPY( szSrcFullPath, pPathName );
        STRCAT( szSrcFullPath, "\\" );
        STRCAT( szSrcFullPath, pFileName );
        
        STRCPY( szDstFullPath, pDestPath ); // THE SAME FOLDER
        STRCAT( szDstFullPath, "\\" );
        STRCAT( szDstFullPath, pDestFileName );
        
        pSrcDb = AHC_DCFDT_GetDBByFolderName((char*)pPathName);
        
        if( pSrcDb == NULL )
        {
            return AHC_FALSE;
        }
        
        psSrcNode = AHC_DCFDT_GetNodeByFilename(pSrcDb, (char*)pFileName );
        
        if( psSrcNode == NULL )
        {
            return AHC_FALSE;
        }
        
        pDstDb = AHC_DCFDT_GetDBByFolderName((char*)pDestPath);
        
        if( pDstDb == NULL )
        {
            return AHC_FALSE;
        }
 
        psDstNode = AHC_DCFDT_GetNodeByFilename(pDstDb, (char*)pDestFileName );
        
        if( psDstNode != NULL )
        {
            AHC_DCFDT_DbRemoveFile(pDstDb, pDestFileName, AHC_TRUE);
        }

        error = AHC_FS_FileDirMove(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));


        if( pDstDb->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
        {
            AIHC_DCFDT_ChangeToNmeaName(szSrcFullPath);
            AIHC_DCFDT_ChangeToNmeaName(szDstFullPath);
            AHC_FS_FileDirMove(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));
        }

        if( error == MMP_ERR_NONE )
        {
            AHC_DCFDT_DbRemoveFile(pSrcDb, pFileName, AHC_FALSE);
            AHC_DCFDT_DbPreAddFile(pDstDb,pDestFileName, AHC_FALSE);
            AHC_DCFDT_DbPostAddFile(pDstDb,pDestFileName);            
        }

    }
    break;
    
    default:
    break;
    }
    
    return AHC_TRUE;
}


AHC_BOOL AHC_DCFDT_SelectDB(DCF_DB_TYPE sType)
{
    //printc("Work DCFDT Type : %d \n", sType);
    
    switch(sType){
    case DCF_DB_TYPE_1ST_DB:
        sDcfDtSet.m_uiCurrDBIdx = 0;
        break;
    #if (DCF_DB_COUNT >= 2)
    case DCF_DB_TYPE_2ND_DB:
        sDcfDtSet.m_uiCurrDBIdx = 1;
        break;
    #endif ///< (Dm_uiDBIndex
    #if (DCF_DB_COUNT >= 3)
    case DCF_DB_TYPE_3RD_DB:
        sDcfDtSet.m_uiCurrDBIdx = 2;
        break;
    #endif///< (DCF_DB_COUNT >= 3)
    
    #if (DCF_DB_COUNT >= 4)
    case DCF_DB_TYPE_4TH_DB:
        sDcfDtSet.m_uiCurrDBIdx = 3;
        break;
    #endif
    
    default:
        printc("[DCFDT ERR] DB NONE \n");
        return AHC_FALSE;
        break;
    }
    sDcfDtSet.m_sDBType = sType;

    return AHC_TRUE;
}

PSDATETIMEDCFDB AHC_DCFDT_GetDBByFolderName(char* szFolderName)
{

    PSDATETIMEDCFDB pDB = NULL;
    INT i;

    for(i=0; i<DCF_DB_COUNT; i++)
    {
        if( 0 == STRCMP(sDcfDtSet.m_sDB[i].szFolderName, szFolderName ) )
        {
            pDB = &sDcfDtSet.m_sDB[i];
            break;
        }
    }

    return pDB;
}

PSDATETIMEDCFDB AIHC_DCFDT_GetDbByType(DCF_DB_TYPE sType)
{
    PSDATETIMEDCFDB pDB = NULL;

    if( sType< DCF_DB_COUNT )
    {
        pDB = &sDcfDtSet.m_sDB[sType];
    }

    return pDB;
}


DCF_DB_TYPE AHC_DCFDT_GetDB(void)
{
    // Get DB Type
    return sDcfDtSet.m_sDBType;
}

//-----------------------------------------------------------
// 
//-----------------------------------------------------------

AHC_BOOL AHC_DCFDT_GetTotalFileCount(UINT32 *puiImgIndex)
{
    PSDATETIMEDCFDB pDB;

    *puiImgIndex    = 0;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        printc("AHC_DCFDT_GetTotalFileCount Failed At DB:%d\r\n", sDcfDtSet.m_uiCurrDBIdx);
        return AHC_FALSE;
    }

    pDB = &(sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx]);

    *puiImgIndex = sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx].ulTotalObjectNum;//pDB->ulTotalObjectNum;

    return AHC_TRUE;
}
//------------------------------------------------------------------------------
//  Function    : AHC_DCFDT_GetTotalRearFileCount
//  Description : This function get Total Rear Cam file count
//------------------------------------------------------------------------------
AHC_BOOL AHC_DCFDT_GetTotalRearFileCount(UINT32 *puiImgIndex)
{
    PSDATETIMEDCFDB pDB;

    *puiImgIndex    = 0;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        printc("AHC_DCFDT_GetTotalFileCount Failed At DB:%d\r\n", sDcfDtSet.m_uiCurrDBIdx);
        return AHC_FALSE;
    }

    pDB = &(sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx]);

    *puiImgIndex = sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx].ulTotalRearCamObjectNum;

    return AHC_TRUE;
}


AHC_BOOL AHC_DCFDT_SetCurrentIndex(UINT32 uiImgIndex)
{

    PSDATETIMEDCFDB pDB;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    if( uiImgIndex >= pDB->ulTotalObjectNum )
    {
        printc("[ERR] AHC_DCFDT_SetCurrentIndex is out of range: %d > %d!\n", uiImgIndex, pDB->ulTotalObjectNum);
        return AHC_FALSE;
    }

    pDB->uiCurrentIndex = uiImgIndex;
    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetCurrentIndex(UINT32 *puiImgIndex)
{
    PSDATETIMEDCFDB pDB;

    *puiImgIndex    = 0;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];
    
    if( pDB->uiCurrentIndex >= pDB->ulTotalObjectNum )
    {
        printc("[ERR] AHC_DCFDT_GetCurrentIndex is out of range!\n");
        pDB->uiCurrentIndex = 0;
    }

    *puiImgIndex = pDB->uiCurrentIndex;
   
    return AHC_TRUE;
}
//------------------------------------------------------------------------------
//  Function    : AHC_DCFDT_SearchNearestRearFileIndex
//  Description : This function search nearest file node which rear cam exist and
//                return the index 
//------------------------------------------------------------------------------
AHC_BOOL AHC_DCFDT_SearchNearestRearFileIndex(UINT32 uiIndex, UINT32 *puiRearFileIndex)
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE pNode;
    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];
    
    if( pDB->uiCurrentIndex >= pDB->ulTotalObjectNum )
    {
        printc("[ERR] AHC_DCFDT_GetCurrentIndex is out of range!\n");
        pDB->uiCurrentIndex = 0;
    }
    
    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex);
	//if this node exist rear cam ,return it
    if(pNode->bIsRearFileExist == AHC_TRUE)
    {
        *puiRearFileIndex = uiIndex;
        return AHC_TRUE;
    }
    else 
    {
        UINT32 uiNodeCount = 0;
        PSDATETIMEFILE pForwardNode  = pNode->pPrev;
        PSDATETIMEFILE pBackwardNode = pNode->pNext;
        //Search file node which exist rearcam file
		//search node one by one,until the head/end of nodes
        while((pForwardNode->pPrev != NULL) || (pBackwardNode->pNext != NULL))
        {
            uiNodeCount++;
            if(pForwardNode->bIsRearFileExist == AHC_TRUE)
            {
                *puiRearFileIndex = uiIndex - uiNodeCount;
                return AHC_TRUE;
            }
            else if(pBackwardNode->bIsRearFileExist == AHC_TRUE)
            {
                *puiRearFileIndex = uiIndex + uiNodeCount;
                return AHC_TRUE;
            }
            else
            {   
                if(pForwardNode->pPrev != NULL){
                    pForwardNode  = pForwardNode->pPrev;
                }
                if(pBackwardNode->pNext != NULL){
                    pBackwardNode = pBackwardNode->pNext;
                }
            }
        }
    }
    printc("1No Rear Cam File exist in this DB\r\n");
    return AHC_FALSE;
}

AHC_BOOL AHC_DCFDT_GetIndexbyName(INT8 *pchDirName,
                                INT8 *pchFileName, 
                                UINT32 *puiIndex)
{
    PSDATETIMEDCFDB pDB;
    UINT            i;

    pDB = AHC_DCFDT_GetDBByFolderName((char*)pchDirName);
    
    if( pDB == NULL )
    {
        return AHC_FALSE;
    }
    
    i = AHC_DCFDT_DbGetIndexByFilename(pDB, (char*)pchDirName );

    if( i == -1 )
    {
        return AHC_FALSE;
    }

    *puiIndex = i;
    return AHC_TRUE;    
}

AHC_BOOL AHC_DCFDT_GetFilePathNamebyIndex(UINT32 uiIndex, char* FilePathName) 
{
    PSDATETIMEDCFDB pDB;
    AHC_BOOL        bRet = AHC_FALSE;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return bRet;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    bRet = AHC_DCFDT_GetFullPathByIndex(pDB, uiIndex, FilePathName, DCF_MAX_FILE_NAME_SIZE );

    return bRet;
}

AHC_BOOL AHC_DCFDT_GetFileNamebyIndex(UINT32 uiIndex, char* FilePathName) 
{
    PSDATETIMEDCFDB pDB;
    AHC_BOOL        bRet = AHC_FALSE;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return bRet;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    bRet = AHC_DCFDT_GetNameByIndex(pDB, uiIndex, FilePathName, DCF_MAX_FILE_NAME_SIZE );

    return bRet;
}

AHC_BOOL AHC_DCFDT_GetFileSizebyIndex(UINT32 uiIndex, UINT32* FileSize) 
{
	#if(0)
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  pNode;
    #endif
    char            szFullPath[DCF_MAX_FILE_NAME_SIZE]={0};

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    AHC_DCFDT_GetFilePathNamebyIndex(uiIndex, szFullPath);
    if(AHC_ERR_NONE != AHC_FS_FileDirGetInfo(szFullPath, 
                                                              STRLEN(szFullPath),
                                                              NULL, 
                                                              NULL,
                                                              FileSize)){
        *FileSize = 0;
        printc("%d GetFileSize Failed!\n", uiIndex);
    }
    #if 0
    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];


    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex);

    if( pNode == NULL )
    {
        return AHC_FALSE;
    }

    *FileSize = pNode->nSizeInCluster * pDB->ulClusterSize;
    #endif
    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetFileTypebyIndex(UINT32 uiIndex, UINT8 *FileType) 
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  pNode;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );

    if( pNode == NULL )
    {
        return AHC_FALSE;
    }

#if (DCF_DB_COUNT == 1)
	if(m_bSetRearCam )
	{
		if(pNode->bIsRearFileExist)
    		*FileType = pNode->byFileType;
		else
			*FileType = DCF_OBG_MAX;
	}
	else
#endif		
	{
		*FileType = pNode->byFileType;
	}

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetFileTimebyIndex(UINT32 uiIndex, AHC_RTC_TIME *pRtcTime) 
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  pNode;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT ||
        pRtcTime == NULL                           )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );

    if( pNode == NULL )
    {
        return AHC_FALSE;
    }

    return AHC_DT_SecondsToDateTime(pNode->nDateTime, pRtcTime);
}

AHC_BOOL AHC_DCFDT_IsReadOnlybyIndex(UINT32 uiIndex, AHC_BOOL *bReadOnly) 
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  pNode;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );

    if( pNode == NULL )
    {
        return AHC_FALSE;
    }

    *bReadOnly = (pNode->bReadOnly)? AHC_TRUE : AHC_FALSE;

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_IsCharLockbyIndex(UINT32 uiIndex, AHC_BOOL *bCharLock) 
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  pNode;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );

    if( pNode == NULL )
    {
        return AHC_FALSE;
    }

    *bCharLock = (pNode->bCharLock)? AHC_TRUE : AHC_FALSE;

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_SetFreeChar(UINT8 byDir, UINT8 byOp, UINT8 byChars[]) 
{

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetAllowedChar(INT8* pbyChar, UINT32 uiLength)
{

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_EnableDB(DCF_DB_TYPE sType, AHC_BOOL bEnable)
{

    if( sType >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    sDcfDtSet.m_bDBEnable[sType] = bEnable;

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetDBStatus(DCF_DB_TYPE sType)
{   
    // return enable or not

    if( sType >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    return sDcfDtSet.m_bDBEnable[sType];

}


AHC_BOOL AHC_DCFDT_WriteInfo(void)
{
    return AHC_TRUE;
}

//Unused
AHC_BOOL AHC_DCFDT_SetCyclicDeletedMask(DCF_FILE_PROTECT sType)
{
    return AHC_TRUE;
}

//Unused
AHC_BOOL AHC_DCFDT_GetCyclicDeletedMask(DCF_FILE_PROTECT* sType)
{

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_SetFileDeletedMask(DCF_FILE_PROTECT sType)
{

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_GetFileDeletedMask(DCF_FILE_PROTECT* sType)
{
    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_DeleteCyclicFilebySize(UINT64 uiSize)
{
    PSDATETIMEDCFDB pDB;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    AIHC_DCFDT_DbDeleteCyclicFilebySize(pDB, uiSize );

    return AHC_TRUE;
}	

AHC_BOOL AHC_DCFDT_MoveFile(DCF_DB_TYPE sFromType, DCF_DB_TYPE sToType, UINT16 uwDirKey, INT8 *pchFileName, AHC_BOOL bReadOnly)
{	
    UINT32              error;

    PSDATETIMEDCFDB pSrcDb = AIHC_DCFDT_GetDbByType(sFromType);
    PSDATETIMEFILE  psSrcNode;
    PSDATETIMEFILE  psDstNode;    
    PSDATETIMEDCFDB pDstDb = AIHC_DCFDT_GetDbByType(sToType);
    char            szSrcFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    char            szDstFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    char            szDstFilename[DCF_MAX_FILE_NAME_SIZE]={0};
    AHC_FS_ATTRIBUTE attrib;
    AHC_BOOL        bRet = AHC_FALSE;
    AHC_BOOL        bRearCamFlag = AHC_FALSE;

    if( pSrcDb == NULL || pDstDb == NULL )
    {
        printc("DCFDT_MoveFile failed!! SrcDb or DstDb = NULL\r\n");
        return bRet;
    }

    STRCPY( szSrcFullPath, pSrcDb->szFolderName );
    STRCAT( szSrcFullPath, "\\" );
    STRCAT( szSrcFullPath, (char*)pchFileName );

    STRCPY( szDstFullPath, pDstDb->szFolderName );
    STRCAT( szDstFullPath, "\\" );


    psSrcNode = AHC_DCFDT_GetNodeByFilename(pSrcDb, (char*)pchFileName );

    if( psSrcNode == NULL )
    {
        printc("DCFDT_MoveFile failed!! Can not get SrcNode!!\r\n");
        return bRet;
    }

    if( !AIHC_DCFDT_CreateFilename(pDstDb, 
                                   psSrcNode->nDateTime, 
                                   psSrcNode->byFileType, 
                                   szDstFilename ))
    {
        printc("DCFDT_MoveFile failed!! CreateFilename Failed!!\r\n");
        return bRet;
    }

    STRCAT( szDstFullPath, (char*)szDstFilename );

    psDstNode = AHC_DCFDT_GetNodeByFilename(pDstDb,szDstFilename);

    if( psDstNode)
    {
        AHC_DCFDT_DbRemoveFile(pDstDb, szDstFilename, AHC_TRUE);
    }
    
    error = AHC_FS_FileDirMove(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));
    if(error != MMP_ERR_NONE)
    {
        printc("DCFDT_MoveFile DirMove failed!! DstFullPath %s\r\n",szDstFullPath);
        return bRet;
    }
    // Read Only or not
    AHC_FS_FileDirGetAttribute(szDstFullPath, 
                               STRLEN(szDstFullPath), &attrib );

    if( bReadOnly )
    {
        attrib |= AHC_FS_ATTR_READ_ONLY;
    }
    else
    {
        attrib &= ~(AHC_FS_ATTR_READ_ONLY);
    }

    AHC_FS_FileDirSetAttribute(szDstFullPath, 
                               STRLEN(szDstFullPath), attrib );

    //insert here
    if(psSrcNode->bIsRearFileExist == AHC_TRUE)
    {
        char szSrcFullPath_2nd[DCF_MAX_FILE_NAME_SIZE] = {0};
        char szSrcFileName_2nd[DCF_MAX_FILE_NAME_SIZE] = {0};
        char szDstFullPath_2nd[DCF_MAX_FILE_NAME_SIZE] = {0};
        char szDstFilename_2nd[DCF_MAX_FILE_NAME_SIZE] = {0};
        AHC_BOOL bSetRearCam;
        bRearCamFlag = AHC_TRUE;
        STRCPY( szSrcFullPath_2nd, pSrcDb->szFolderName_2nd );
        STRCAT( szSrcFullPath_2nd, "\\" );

        STRCPY( szDstFullPath_2nd, pDstDb->szFolderName_2nd );
        STRCAT( szDstFullPath_2nd, "\\" );
        
        //set flag true
        AHC_DCFDT_GetRearCamPathFlag(&bSetRearCam);
        AHC_DCFDT_SetRearCamPathFlag(AHC_TRUE);
        //create SrcFileName_2nd
        if( !AIHC_DCFDT_CreateFilename(pSrcDb,
                                  psSrcNode->nDateTime, 
                                  psSrcNode->byFileType, 
                                  szSrcFileName_2nd ))
        {
            printc("1Create SrcFileName_2nd Failed!\r\n");
            return bRet;
        }
        //create DstFileName_2nd
        if( !AIHC_DCFDT_CreateFilename(pDstDb, 
                                   psSrcNode->nDateTime, 
                                   psSrcNode->byFileType, 
                                   szDstFilename_2nd ))
        {
            printc("1Create DstFileName_2nd Failed!\r\n");
            return bRet;
        }
        //set flag as previous status 
        AHC_DCFDT_SetRearCamPathFlag(bSetRearCam);
        //
        STRCAT( szSrcFullPath_2nd, (char*)szSrcFileName_2nd );
        
        STRCAT( szDstFullPath_2nd, (char*)szDstFilename_2nd );
        printc("szSrcFullPath_2nd %s\r\n",szSrcFullPath_2nd);
        printc("szDstFullPath_2nd %s\r\n",szDstFullPath_2nd);
        error |= AHC_FS_FileDirMove(szSrcFullPath_2nd, 
                                    STRLEN(szSrcFullPath_2nd), 
                                    szDstFullPath_2nd, 
                                    STRLEN(szDstFullPath_2nd));
        // Read Only or not
        AHC_FS_FileDirGetAttribute(szDstFullPath_2nd, 
                                   STRLEN(szDstFullPath_2nd), 
                                   &attrib );
        if( bReadOnly )
        {
            attrib |= AHC_FS_ATTR_READ_ONLY;
        }
        else
        {
            attrib &= ~(AHC_FS_ATTR_READ_ONLY);
        }
        AHC_FS_FileDirSetAttribute(szDstFullPath_2nd, 
                                   STRLEN(szDstFullPath_2nd), 
                                   attrib );
    }
    
    if( error == MMP_ERR_NONE )
    {
        AHC_DCFDT_DbRemoveFile(pSrcDb, pchFileName, AHC_FALSE);

        //set flag true
        if(bRearCamFlag)
        {
            m_bCreateRearCamFile = AHC_TRUE;
        }
        AHC_DCFDT_DbPreAddFile(pDstDb, szDstFilename, AHC_FALSE);
        AHC_DCFDT_DbPostAddFile(pDstDb,szDstFilename);            
        bRet = AHC_TRUE;
    }

    if( pSrcDb->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
    {
        AIHC_DCFDT_ChangeToNmeaName(szSrcFullPath);
        AIHC_DCFDT_ChangeToNmeaName(szDstFullPath);
        AHC_FS_FileRemove(szDstFullPath, STRLEN(szDstFullPath));
        AHC_FS_FileDirMove(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));
    }

    return bRet;

}

AHC_BOOL AHC_DCFDT_RenameFile(DCF_DB_TYPE sFromType, AHC_RTC_TIME* psRtcTimeOld, AHC_RTC_TIME* psRtcTimeNew)
{	
    DATE_TIME       nDateTimeOld, nDateTimeNew;
    char            szSrcFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    char            szDstFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    char            szSrcRearFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    char            szDstRearFullPath[DCF_MAX_FILE_NAME_SIZE]={0};
    UINT32          uiFolderNameLen,uiRearFolderNameLen;
    PSDATETIMEDCFDB pSrcDb = AIHC_DCFDT_GetDbByType(sFromType);

    PSDATETIMEFILE  psSrcNode;
    PSDATETIMEFILE  psDstNode;    
    AHC_BOOL        bRet = AHC_FALSE;
    AHC_BOOL        bIsRearFileExist = AHC_FALSE;

    if( pSrcDb == NULL )
    {
        return bRet;
    }

    AHC_DT_GetSecondsOffsetFromRtc(psRtcTimeOld,&nDateTimeOld);
    AHC_DT_GetSecondsOffsetFromRtc(psRtcTimeNew,&nDateTimeNew);

    psSrcNode = AHC_DCFDT_GetNodeByDateTime(pSrcDb,nDateTimeOld);

    if( psSrcNode == NULL )
    {
        return bRet;
    }
    //rename FrontCam
    if(psSrcNode->bIsRearFileExist){
        bIsRearFileExist = AHC_TRUE;
    }
    bRet = AHC_DCFDT_RenameFilebyNode(pSrcDb, psSrcNode, 
                                      szSrcFullPath, szDstFullPath,
                                      nDateTimeOld, nDateTimeNew,
                                      &uiFolderNameLen,AHC_FALSE);
    //if node exist
    psDstNode = AHC_DCFDT_GetNodeByDateTime(pSrcDb,nDateTimeNew); 
    if(psDstNode)
    {
        AHC_DCFDT_DbRemoveFileByNode(pSrcDb, psDstNode, AHC_TRUE);
    }
    //RearCam
    if(bIsRearFileExist)
    {
        AHC_DCFDT_RenameFilebyNode(pSrcDb, psSrcNode, 
                                   szSrcRearFullPath, szDstRearFullPath,
                                   nDateTimeOld, nDateTimeNew, 
                                   &uiRearFolderNameLen, AHC_TRUE);
        m_bCreateRearCamFile = AHC_TRUE;
    }   

    AHC_DCFDT_DbRemoveFile (pSrcDb, szSrcFullPath+uiFolderNameLen, AHC_FALSE);
    AHC_DCFDT_DbPreAddFile (pSrcDb, szDstFullPath+uiFolderNameLen, AHC_FALSE);
    AHC_DCFDT_DbPostAddFile(pSrcDb, szDstFullPath+uiFolderNameLen);
    //NMEA
    if( pSrcDb->uiFlag & DCFDT_DB_FLAG_HAS_NMEA_FILE )
    {
        AIHC_DCFDT_ChangeToNmeaName(szSrcFullPath);
        AIHC_DCFDT_ChangeToNmeaName(szDstFullPath);
        AHC_FS_FileRemove(szDstFullPath, STRLEN(szDstFullPath));
        AHC_FS_FileDirRename(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));
    }
    return bRet;
}
AHC_BOOL AHC_DCFDT_RenameFilebyNode(PSDATETIMEDCFDB pSrcDb, PSDATETIMEFILE  psSrcNode, 
                                    char* szSrcFullPath   , char* szDstFullPath,
                                    DATE_TIME nDateTimeOld, DATE_TIME nDateTimeNew, 
                                    UINT32 *puiFolderNameLen, AHC_BOOL bIsRearCam)
{
    UINT32          error;
    AHC_BOOL        bRet = AHC_FALSE;
    char            szFolderName[DCF_MAX_FILE_NAME_SIZE]={0};
    AHC_BOOL        bRestoreSetRearCam = AHC_FALSE; //restore SetRearCam after create filename
    if(bIsRearCam){
        MEMCPY(szFolderName, pSrcDb->szFolderName_2nd, sizeof(pSrcDb->szFolderName_2nd));
    }else{
        MEMCPY(szFolderName, pSrcDb->szFolderName, sizeof(pSrcDb->szFolderName));
    }
    STRCPY( szSrcFullPath, szFolderName );
    STRCAT( szSrcFullPath, "\\" );
    *puiFolderNameLen = STRLEN(szSrcFullPath);
    bRestoreSetRearCam = m_bSetRearCam;
    m_bSetRearCam = bIsRearCam;
    if( !AIHC_DCFDT_CreateFilename(pSrcDb, 
                                   nDateTimeOld, 
                                   psSrcNode->byFileType, 
                                   szSrcFullPath + (*puiFolderNameLen) ))
    {
        
        return bRet;
    }

    STRCPY( szDstFullPath, szFolderName );
    STRCAT( szDstFullPath, "\\" );

    if( !AIHC_DCFDT_CreateFilename(pSrcDb, 
                                   nDateTimeNew, 
                                   psSrcNode->byFileType, 
                                   szDstFullPath + (*puiFolderNameLen) ))
    {
        return bRet;
    }
    m_bSetRearCam = bRestoreSetRearCam;

    error = AHC_FS_FileDirRename(szSrcFullPath, STRLEN(szSrcFullPath), szDstFullPath, STRLEN(szDstFullPath));


    if( error == MMP_ERR_NONE ){
        bRet = AHC_TRUE;
    }else{
        printc("!!!error = 0x%x, %s\r\n", error, __func__);
    }
    
    return bRet;
}

AHC_BOOL AHC_DCFDT_UpdateLastFiles(DCF_DB_TYPE sDB)
{
    PSDATETIMEDCFDB pDB = AIHC_DCFDT_GetDbByType(sDB);
    return AHC_DCFDT_DbUpdateLastFiles(pDB);
}


UINT32 AHC_DCFDT_GetCyclicCluster(DCF_DB_TYPE sDB)
{
    PSDATETIMEDCFDB pDB = AIHC_DCFDT_GetDbByType(sDB);
    return AHC_DCFDT_DbGetCyclicCluster(pDB);
}

UINT32 AHC_DCFDT_GetFileCluster(DCF_DB_TYPE sDB)
{
    PSDATETIMEDCFDB pDB = AIHC_DCFDT_GetDbByType(sDB);
    UINT32 uiClusters;
    uiClusters = AHC_DCFDT_DbGetFileCluster(pDB);
    printc("DCF_File%d =%d\n", sDB, uiClusters);
    return uiClusters;
}

UINT32 AHC_DCFDT_GetClusterTH(DCF_DB_TYPE sDB)
{	
    PSDATETIMEDCFDB pDB = AIHC_DCFDT_GetDbByType(sDB);
    UINT32 uiClusters;
    uiClusters = AHC_DCFDT_DbGetClusterTH(pDB);
    printc("DCF_Th%d = %d\n", sDB, uiClusters);
    return uiClusters;
}

AHC_BOOL AIHC_DCFDT_PreAddFile(UINT16 uwDirKey, INT8 *pchFileName)
{
    PSDATETIMEDCFDB pDB;
    AHC_BOOL        bDelOldFile = AHC_FALSE;

#if DCFDT_DELETE_OLD_DUPLICATE
    bDelOldFile = AHC_TRUE;
#endif
    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];


    return AHC_DCFDT_DbPreAddFile(pDB, pchFileName, bDelOldFile ); 

}

AHC_BOOL AIHC_DCFDT_PostAddFile(UINT16 uwDirKey, INT8 *pchFileName)
{
    PSDATETIMEDCFDB pDB;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    return AHC_DCFDT_DbPostAddFile(pDB, pchFileName ); 
    
}


AHC_BOOL AIHC_DCFDT_MountAllDB(UINT32 uiMaxMemSize)
{
    INT i;
    UINT32 uiClustersInDb = 0;
    UINT32 uiPayloadClusters;
    // Init memory pool
    AHC_DCFDT_MemPoolFSInit(uiMaxMemSize);

    if(MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)
    {
        // Init settingS
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[0], DCFDT_DB0_FOLDER,  DCFDT_DB0_FILENAME_TEMPLATE,  DCFDT_DB0_FILENAME_LEN, DCFDT_DB0_FLAG);
        AHC_DCFDT_InitRearCamPara(&sDcfDtSet.m_sDB[0], DCFDT_DB0_REAR_FOLDER, DCFDT_DB0_REAR_FILENAME_TEMPLATE);
        #if (DCF_DB_COUNT >= 2)
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[1], DCFDT_DB1_FOLDER,  DCFDT_DB1_FILENAME_TEMPLATE,  DCFDT_DB1_FILENAME_LEN, DCFDT_DB1_FLAG);
        AHC_DCFDT_InitRearCamPara(&sDcfDtSet.m_sDB[1], DCFDT_DB1_REAR_FOLDER, DCFDT_DB1_REAR_FILENAME_TEMPLATE);
        #endif //DCF_DB_COUNT >= 2

        #if (DCF_DB_COUNT >= 3)
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[2], DCFDT_DB2_FOLDER,  DCFDT_DB2_FILENAME_TEMPLATE,  DCFDT_DB2_FILENAME_LEN, DCFDT_DB2_FLAG);
        AHC_DCFDT_InitRearCamPara(&sDcfDtSet.m_sDB[2], DCFDT_DB2_REAR_FOLDER, DCFDT_DB2_REAR_FILENAME_TEMPLATE);
        #endif //DCF_DB_COUNT >= 3

        #if (DCF_DB_COUNT >= 4)
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[3], DCFDT_DB3_FOLDER,  DCFDT_DB3_FILENAME_TEMPLATE,  DCFDT_DB3_FILENAME_LEN, DCFDT_DB3_FLAG);
        AHC_DCFDT_InitRearCamPara(&sDcfDtSet.m_sDB[3], DCFDT_DB3_REAR_FOLDER, DCFDT_DB3_REAR_FILENAME_TEMPLATE);
        #endif //DCF_DB_COUNT >= 3
    }else if(MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD1)
    {
        // Init settingS
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[0], DCFDT_DB0_FOLDER_SD1,  DCFDT_DB0_FILENAME_TEMPLATE,  DCFDT_DB0_FILENAME_LEN, DCFDT_DB0_FLAG);

        #if (DCF_DB_COUNT >= 2)
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[1], DCFDT_DB1_FOLDER_SD1,  DCFDT_DB1_FILENAME_TEMPLATE,  DCFDT_DB1_FILENAME_LEN, DCFDT_DB1_FLAG);
        #endif //DCF_DB_COUNT >= 2

        #if (DCF_DB_COUNT >= 3)
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[2], DCFDT_DB2_FOLDER_SD1,  DCFDT_DB2_FILENAME_TEMPLATE,  DCFDT_DB2_FILENAME_LEN, DCFDT_DB2_FLAG);
        #endif //DCF_DB_COUNT >= 3

        #if (DCF_DB_COUNT >= 4)
        AHC_DCFDT_InitDB(&sDcfDtSet.m_sDB[3], DCFDT_DB3_FOLDER_SD1,  DCFDT_DB3_FILENAME_TEMPLATE,  DCFDT_DB3_FILENAME_LEN, DCFDT_DB3_FLAG);
        #endif //DCF_DB_COUNT >= 3
    }

    // Mount each db
    for(i=0; i<DCF_DB_COUNT; i++)
    {
        if( sDcfDtSet.m_bDBEnable[i] )
        {
            AHC_DCFDT_MountDB(&sDcfDtSet.m_sDB[i]);
            uiClustersInDb += sDcfDtSet.m_sDB[i].ullTotalObjectSize/sDcfDtSet.m_sDB[i].ulClusterSize;
            sDcfDtSet.m_bMountDCF[i] = AHC_TRUE;
        }
    }

    // set ALLOW_TYPE?

    // Threshold setting
    {
        UINT8 nSetting =0 ;
        if( AHC_DCFDT_CB_GetStorageAllocationSetting )
        {
            AHC_DCFDT_CB_GetStorageAllocationSetting(&nSetting);
        }
        
        AHC_DCFDT_AdjustAllocation(nSetting);
    }
    
    //  printc("TotalCluster    : %d, \r\n", m_sMediaInfo.ulTotalClusters);
    //  printc("FreelCluster    : %d, \r\n", m_sMediaInfo.ulFreeClusters);  
    uiPayloadClusters = m_sMediaInfo.ulTotalClusters 
                        - m_sMediaInfo.ulFreeClusters
                        - uiClustersInDb;

    for(i=0; i<DCF_DB_COUNT; i++)
    {
        if( sDcfDtSet.m_sDB[i].bThreadConsiderPayload )
        {
            sDcfDtSet.m_sDB[i].ulPayloadInCluster = uiPayloadClusters;
            break;
        }
    }

    // Check if need to create device.xml
    #if ( UUID_XML_FEATURE == 1 )
    {
        if(MenuSettingConfig()->uiMediaSelect == MEDIA_SETTING_SD_CARD)
        {
            if( !UI_WriteSdUuidXML() )
            {
                return AHC_FALSE;
            }
        }
    }
    #endif

    // Dump
#if (DCFDT_DEBUG)    
    for(i=0; i<DCF_DB_COUNT; i++)
    {
        AHC_DCFDT_DumpDb(&sDcfDtSet.m_sDB[i]);
    }
#endif
    // Get Payload size
    // Set payload to DB

    return AHC_TRUE;
}

AHC_BOOL AIHC_DCFDT_UnmountAllDB(void)
{
    INT i;
    for(i=0; i<DCF_DB_COUNT; i++)
    {

        if( sDcfDtSet.m_bMountDCF[i])
        {
            AHC_DCFDT_UnmountDB(&sDcfDtSet.m_sDB[i]);
            sDcfDtSet.m_bMountDCF[i] = AHC_FALSE;
        }
    }

    AHC_DCFDT_MemPoolFSUninit();

    return AHC_TRUE;
}

AHC_BOOL AHC_DCFDT_IsDBMounted(void)
{

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    return sDcfDtSet.m_bMountDCF[sDcfDtSet.m_uiCurrDBIdx];
   
}

AHC_BOOL AHC_DCFDT_GetName(AHC_RTC_TIME* psRtcTime, INT8 *FileFullPath, INT8 *FileName, UINT8 *bCreateNewDir)
{
    PSDATETIMEDCFDB pDB;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    *bCreateNewDir = AHC_FALSE;

    if(m_bSetRearCam == AHC_TRUE)
    {
        m_bCreateRearCamFile = AHC_TRUE;
    }

    return AHC_DCFDT_GetFullPath(pDB, psRtcTime, (char*)FileFullPath, (char*)FileName);

    //return AHC_DCFDT_GetBaseName(pDB, &sRtcTime, FileName, DCF_MAX_FILE_NAME_SIZE );
}

void AIHC_DCFDT_DumpDB(void)
{
    INT i;
    for(i=0; i<DCF_DB_COUNT; i++)
    {
        printc("\n\nDB[%d]\n", i );
        AHC_DCFDT_DumpDb(&sDcfDtSet.m_sDB[i]);
    }

    printc("Total Clusters = %d\n", m_sMediaInfo.ulTotalClusters );

}

AHC_BOOL AHC_DCFDT_AdjustAllocation(DCF_STORAGE_ALLOC eType)
{
    UINT32 uiNumeratorDB[4];
    UINT32 uiDenominator;
    INT i;
    
    switch(eType){

    case DCF_STORAGE_ALLOC_TYPE1:
        uiNumeratorDB[0] = DCF_CLUSTER_NUMERATOR1_DB0;
        uiNumeratorDB[1] = DCF_CLUSTER_NUMERATOR1_DB1;
        uiNumeratorDB[2] = DCF_CLUSTER_NUMERATOR1_DB2;
        uiNumeratorDB[3] = DCF_CLUSTER_NUMERATOR1_DB3;
        uiDenominator = DCF_CLUSTER_DENOMINATOR;
        break;
    
    case DCF_STORAGE_ALLOC_TYPE2:
        uiNumeratorDB[0] = DCF_CLUSTER_NUMERATOR2_DB0;
        uiNumeratorDB[1] = DCF_CLUSTER_NUMERATOR2_DB1;
        uiNumeratorDB[2] = DCF_CLUSTER_NUMERATOR2_DB2;
        uiNumeratorDB[3] = DCF_CLUSTER_NUMERATOR2_DB3;
        uiDenominator = DCF_CLUSTER_DENOMINATOR;
        break;
        
    case DCF_STORAGE_ALLOC_TYPE3:
        uiNumeratorDB[0] = DCF_CLUSTER_NUMERATOR3_DB0;
        uiNumeratorDB[1] = DCF_CLUSTER_NUMERATOR3_DB1;
        uiNumeratorDB[2] = DCF_CLUSTER_NUMERATOR3_DB2;
        uiNumeratorDB[3] = DCF_CLUSTER_NUMERATOR3_DB3;
        uiDenominator = DCF_CLUSTER_DENOMINATOR;
        break;
        
    case DCF_STORAGE_ALLOC_TYPE0:
    default:
        uiNumeratorDB[0] = DCF_CLUSTER_NUMERATOR_DB0;
        uiNumeratorDB[1] = DCF_CLUSTER_NUMERATOR_DB1;
        uiNumeratorDB[2] = DCF_CLUSTER_NUMERATOR_DB2;
        uiNumeratorDB[3] = DCF_CLUSTER_NUMERATOR_DB3;
        uiDenominator = DCF_CLUSTER_DENOMINATOR;
        break;
    }
    
    m_AllocType = eType;

    for( i=0; i< DCF_DB_COUNT; i++)
    {
        sDcfDtSet.m_sDB[i].ulMaxSizeThresholdInCluster = m_sMediaInfo.ulTotalClusters *uiNumeratorDB[i]/uiDenominator;
        //printc("DB[%d].ulMaxSizeThresholdInCluster=%d\r\n", i, sDcfDtSet.m_sDB[i].ulMaxSizeThresholdInCluster);
    }

    return AHC_TRUE;
}

UINT64 AHC_DCFDT_GetFileTH(DCF_STORAGE_ALLOC sType)
{
    switch(sType){
        case DCF_STORAGE_ALLOC_TYPE0:
            return DCF_FILE_NUMERATOR2_DB0;
            break;
         case DCF_STORAGE_ALLOC_TYPE1:
            return DCF_FILE_NUMERATOR2_DB1;
            break;
        case DCF_STORAGE_ALLOC_TYPE2:
            return DCF_FILE_NUMERATOR2_DB2;
            break;
         case DCF_STORAGE_ALLOC_TYPE3:
            return DCF_FILE_NUMERATOR2_DB3;
            break;
        default:
            return DCF_FILE_NUMERATOR2_DB0;
            break;
    
    }
}
void AHC_DCFDT_Test(void)
{
    // TODO: Add your control notification handler code here
    //-------------------------------------
    // Init DB
    // Mount DB
    // 
    //-------------------------------------
    if(1)
    {
    
        AIHC_DCFDT_MountAllDB(1024*1024);

        //AHC_DCFDT_GetFileExtension( "ABCDEFGabc", szEXT, 4 );
        //AHC_DCFDT_GetFileInfo(NULL, "ABCDEFG.AVI");
        
    }    
    
}
void AHC_DCFDT_SetRearCamPathFlag(AHC_BOOL bSetRearCam)
{
#if DCF_DUAL_CAM_ENABLE
    m_bSetRearCam = bSetRearCam;
#else
    printc("DCF_DUAL_CAM_ENABLE Disabled!\r\n");
#endif
}
void AHC_DCFDT_GetRearCamPathFlag(AHC_BOOL *bGetRearCam)
{
#if DCF_DUAL_CAM_ENABLE
        *bGetRearCam = m_bSetRearCam ;
#else
        printc("DCF_DUAL_CAM_ENABLE Disabled!\r\n");
#endif
}

AHC_BOOL AHC_DCFDT_IsRearCamFileExistbyIndex(UINT32 uiIndex, AHC_BOOL *bFileExist) 
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  pNode;

    if( sDcfDtSet.m_uiCurrDBIdx >= DCF_DB_COUNT )
    {
        return AHC_FALSE;
    }

    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];

    pNode = AIHC_DCFDT_GetNodeByIndex(pDB, uiIndex );

    if( pNode == NULL )
    {
        *bFileExist = AHC_FALSE;
        return AHC_FALSE;
    }

    *bFileExist = (pNode->bIsRearFileExist)? AHC_TRUE : AHC_FALSE;

    return AHC_TRUE;
}
//------------------------------------------------------------------------------
//  Function    : AHC_DCFDT_RearIndexMappingToNormalIndex
//  Description : This function maps rear cam Index(rear cam playback mode) 
//                to DCF normal index
//                (e.g. DCF Index from 0 ~ 9 , rear cam exist 0,1,8,9(totalnum = 4) 
//,               Given input 3(rear Index), this function would return 9(DCF Index))
//------------------------------------------------------------------------------
//for rear cam playback rearcam DCF index mapping to DCF index
AHC_BOOL AHC_DCFDT_RearIndexMappingToNormalIndex(UINT32 uiRearIndex, UINT32 *puiIndex)
{
    AHC_BOOL bFileExist;
    UINT32 i;
    UINT32 uiTotalFileNum;
    UINT32 uiRearFileCount = 0;

    AHC_DCFDT_GetTotalFileCount(&uiTotalFileNum);
    for(i = 0; i < uiTotalFileNum; i++)
    {
        AHC_DCFDT_IsRearCamFileExistbyIndex(i, &bFileExist);
        if(bFileExist)
        {
            uiRearFileCount++;
            if((uiRearFileCount-1) == uiRearIndex )
            {
                *puiIndex = i;
                return AHC_TRUE;
            }
        }
    }
    
    if(*puiIndex == 0)
    {
        printc("No Rear Cam files exist in this DB %d\r\n",sDcfDtSet.m_uiCurrDBIdx);
    }
    else
    {
        printc("RearIndex exceed rear total num!! %d",uiRearIndex);
    }
    return AHC_FALSE;
}
//------------------------------------------------------------------------------
//  Function    : AHC_DCFDT_NormalIndexMappingToRearIndex
//  Description : This function maps DCF normal index 
//                to rear cam Index(rear cam playback mode)
//                (e.g. DCF Index from 0 ~ 9 , rear cam exist 0,1,8,9(totalnum = 4) 
//,               Given input 8(DCF Index), this function would return 2(rear Index))
//------------------------------------------------------------------------------
AHC_BOOL AHC_DCFDT_NormalIndexMappingToRearIndex(UINT32 puiIndex, UINT32 *uiRearIndex)
{
    AHC_BOOL bFileExist;
    UINT32 uiTotalFileNum;
    UINT32 uiRearFileCount = 0;
    UINT32 i;
    
    AHC_DCFDT_GetTotalFileCount(&uiTotalFileNum);
    if((puiIndex+1) > uiTotalFileNum){
        printc("puiIndex %d exceed TotalFileNum %d\r\n",puiIndex,uiTotalFileNum);
        return AHC_FALSE;
    }
    
    for(i = 0; i < (puiIndex+1); i++)
    {
        AHC_DCFDT_IsRearCamFileExistbyIndex(i, &bFileExist);
        if(bFileExist)
        {
            uiRearFileCount++;
        }
    }
    
    if(uiRearFileCount == 0){
        printc("No Rear Cam file in this DB%d\r\n",sDcfDtSet.m_uiCurrDBIdx);     
    }
    else if(uiRearFileCount > uiTotalFileNum){
        printc("RearFileCount exceed TotalFileNum\r\n");
    }
    else{
        *uiRearIndex = uiRearFileCount - 1;
        return AHC_TRUE;
    }
    return AHC_FALSE;
}

#if (FS_FORMAT_FREE_ENABLE)
AHC_BOOL AHC_DCFDT_SearchAvailableFileSlot(char* szFilePathName)
{
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE psNode;
    AHC_BOOL bFirstHiddenFile = AHC_TRUE;
    
    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];
    
    if( pDB->pFileHead == NULL )
    {
        printc("There's no file node !!\n");
        return AHC_FALSE;
    }
    else
    {
        PSDATETIMEFILE psNodeCurr = pDB->pFileHead;
        while( psNodeCurr )
        {
            if(psNodeCurr->bHidden)
            {
                PSDATETIMEFILE psNodePrev = psNodeCurr->pPrev;
                PSDATETIMEFILE psNodeNext = psNodeCurr->pNext;

                #if 0 //Enable it as workaround to avoid broken file at 1st position
                if( bFirstHiddenFile == AHC_TRUE )
                {
                    bFirstHiddenFile = AHC_FALSE;
                    psNodeCurr = psNodeCurr->pNext;
                    continue;
                }
                #endif

                AHC_DCFDT_GetFullPathByNode(pDB, psNodeCurr, szFilePathName, DCF_MAX_FILE_NAME_SIZE);

                if(psNodePrev == NULL)
                { //The first Node
                    pDB->pFileHead = psNodeNext;
                    psNodeNext->pPrev = NULL;
                }
                else if(psNodeNext == NULL)
                { //The last Node
                    psNodePrev->pNext = NULL;
                }
                else {//The middle Node
                    psNodeNext->pPrev = psNodePrev;
                    psNodePrev->pNext = psNodeNext;
                }
                return AHC_TRUE;
            }
            psNodeCurr = psNodeCurr->pNext;
        }
        //if there's no hidden file, search the pFileHead(the earliest file)
        if( psNodeCurr == NULL )
        {
            psNodeCurr = pDB->pFileHead;
            while( psNodeCurr )
            {
                if(!psNodeCurr->bHidden)
                {
                    PSDATETIMEFILE psNodePrev = psNodeCurr->pPrev;
                    PSDATETIMEFILE psNodeNext = psNodeCurr->pNext;
                    AHC_DCFDT_GetFullPathByNode(pDB, psNodeCurr, szFilePathName, DCF_MAX_FILE_NAME_SIZE);
                    if(psNodePrev == NULL)
                    { //The first Node
                        pDB->pFileHead = psNodeNext;
                        psNodeNext->pPrev = NULL;
                    }
                    else if(psNodeNext == NULL)
                    { //The last Node
                        psNodePrev->pNext = NULL;
                    }
                    else {//The middle Node
                        psNodeNext->pPrev = psNodePrev;
                        psNodePrev->pNext = psNodeNext;
                    }
                    return AHC_TRUE;
                }
                psNodeCurr = psNodeCurr->pNext;
            }
        }
    }
    return AHC_FALSE;
}
#endif

AHC_BOOL AHC_DCFDT_TemplateCmp( char* pInput, char* pTemplate )
{  
    while( *pTemplate && *pInput )
    {
        if( *pTemplate == '%' )
        {          	
            pTemplate++;
            while(*pTemplate)
            {
                pInput++;
                pTemplate++;
                if( (*pTemplate == 'X') || (*pTemplate == 'x') || (*pTemplate == 'd')
                 || (*pTemplate == 'z') || (*pTemplate == 's') )
                 {
                    pTemplate++;
                    break;
                 }
            }
        }
        else if( *pTemplate == *pInput )
        {
            pInput++;
            pTemplate++;
        }
        else
        {
            return AHC_FALSE;
        }
    }

    return AHC_TRUE;
}

INT8* AHC_DCFDT_GetRootDirName(void)
{
    PSDATETIMEDCFDB pDB;
    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];
    if(m_bSetRearCam == AHC_TRUE)
    {
        return pDB->szFolderName_2nd;
    }
    else
    {
        return pDB->szFolderName;
    }    
}
//for DCFDT inside use
void AIHC_DCFDT_UpdateLockFileTable(PSDATETIMEFILE psNode, AHC_BOOL *bDeleteFile, UINT8 byOperation)
{
    #define LOCKFILETABLE_EMPTY 0x0
    UINT32 i, j, k;
    PSDATETIMEDCFDB pDB;
    PSDATETIMEFILE  psDelNode = NULL;
    UINT32          ulLockFileNum;
    AHC_BOOL        bRet;
    AHC_BOOL        bSort = AHC_FALSE;
    
    *bDeleteFile = AHC_FALSE;
    pDB = &sDcfDtSet.m_sDB[sDcfDtSet.m_uiCurrDBIdx];
    //update node into table
    switch(byOperation)
    {
    //add node to lock table
    case DCFDT_LOCKTABLE_LOCK:    
    {
        for( i = 0; i < MAX_LOCK_FILE_NUM; i ++)
        {   
            //if repeated node, do not update
            if(pDB->ulCharLockFileTable[i] == (UINT32)psNode)
            {
                printc("Repeated LockFile\r\n");
                //pDB->ulCharLockFileTable[i] = LOCKFILETABLE_EMPTY;
                break;
            }
            //put node into empty slot
            if(pDB->ulCharLockFileTable[i] == LOCKFILETABLE_EMPTY)
            {
                pDB->ulCharLockFileTable[i] = (UINT32)psNode;
                ulLockFileNum = i;
                bSort = AHC_TRUE;
                break;
            }
            //if there's no slot in this table
            if(i == (MAX_LOCK_FILE_NUM - 1) && pDB->ulCharLockFileTable[i] != LOCKFILETABLE_EMPTY)
            {
                PSDATETIMEFILE psCurNode;
                //compare all node datetime in table
                UINT32 uiCmpCount = 0; 
                for(j = 0; j < MAX_LOCK_FILE_NUM; j++)
                {
                    psCurNode = (PSDATETIMEFILE)pDB->ulCharLockFileTable[j];
                    if(psCurNode->nDateTime > psNode->nDateTime)
                    { 
                        uiCmpCount++;
                    }
                }
                //if add node Dateime smaller than all node in table, delete add node
                if(uiCmpCount == MAX_LOCK_FILE_NUM)
                {
                    psDelNode = psNode;
                }
                else
                {
                    psDelNode = (PSDATETIMEFILE)pDB->ulCharLockFileTable[0];
                    //shift addr from j+1 to j
                    for(j = 0; j < MAX_LOCK_FILE_NUM-1; j++)
                    {
                        pDB->ulCharLockFileTable[j] = pDB->ulCharLockFileTable[j+1];
                    }
                    pDB->ulCharLockFileTable[i] = (UINT32)psNode;
                    ulLockFileNum = MAX_LOCK_FILE_NUM;
                    bSort = AHC_TRUE;
                }
            }
        }
    
        //Bobble Sort
        if(bSort)
        {
            AHC_BOOL bSwap = AHC_TRUE;
            PSDATETIMEFILE psCurNodePlus,psCurNode;
            UINT32         ulTmpNodeAddr;
            
            //Bobble Sort    
            for(j = 0; j < ulLockFileNum && bSwap; j++)
            {
                bSwap = AHC_FALSE;
                for(k = 0; k < ulLockFileNum - j - 1; k++)
                {
                    psCurNodePlus = (PSDATETIMEFILE)pDB->ulCharLockFileTable[k+1];
                    psCurNode     = (PSDATETIMEFILE)pDB->ulCharLockFileTable[k];
                    if(psCurNodePlus->nDateTime < psCurNode->nDateTime)
                    {
                        //swap
                        ulTmpNodeAddr = pDB->ulCharLockFileTable[k+1];
                        pDB->ulCharLockFileTable[k+1] = pDB->ulCharLockFileTable[k];
                        pDB->ulCharLockFileTable[k]   = ulTmpNodeAddr;
                        bSwap = AHC_TRUE;
                    }
                }
            }
        }

        if(psDelNode != NULL)
        {
            printc("delete file\r\n");
            bRet = AHC_DCFDT_DbRemoveFileByNode(pDB, psDelNode, AHC_TRUE);
            if(!bRet){
                printc("Remove node error!!\r\n");
            }
            *bDeleteFile = AHC_TRUE;
        }
    }
    break;
    //unlock node from lock table, rollback to normal file, so not need to delete file
    case DCFDT_LOCKTABLE_UNLOCK:
    {
        AHC_BOOL bFound = AHC_FALSE;
        for( i = 0; i < MAX_LOCK_FILE_NUM; i ++)
        {
            if(pDB->ulCharLockFileTable[i] == (UINT32)psNode)
            {
                bFound = AHC_TRUE;
                //shift addr from j+1 to j
                for(j = i; j < MAX_LOCK_FILE_NUM-1; j++)
                {
                    pDB->ulCharLockFileTable[j] = pDB->ulCharLockFileTable[j+1];
                }
                //set last addr = LOCKFILETABLE_EMPTY
                pDB->ulCharLockFileTable[MAX_LOCK_FILE_NUM - 1] = LOCKFILETABLE_EMPTY;
                break;
            }
        }
        if(!bFound){
            printc("Unlock failed!! There's no node matched!!\r\n");
        }
    }
    break;
    default:
    break;
    }
}


#endif

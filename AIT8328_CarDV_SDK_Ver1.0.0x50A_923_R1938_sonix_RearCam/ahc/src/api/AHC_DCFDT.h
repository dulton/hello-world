//==============================================================================
//
//  File        : AHC_DCFDT.c
//  Description : INCLUDE File for the AHC DCF function porting.
//  Author      : Caesar Chang
//  Revision    : 1.0
//
//==============================================================================
#include "AHC_OS.h"
#include "AHC_DCF.h"
#ifndef _AHC_DTDCF_H_
#define _AHC_DTDCF_H_

/*===========================================================================
 * Include files
 *===========================================================================*/

#include "AHC_Common.h"
#include "AHC_Config_SDK.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/


/*===========================================================================
 * Structure define
 *===========================================================================*/

/*===========================================================================
 * Enum define
 *===========================================================================*/
#define RECORD_FILE_SIZE        1

// AHC_DCF_TEMP_BASE_ADDR
#define AHC_DTDCF_MEM_START		AHC_DCF_TEMP_BASE_ADDR //0x4C00000 //0x4E00000

#define SECONDS_PER_DAY         (24*60*60)
#define SECONDS_PER_HOUR        (60*60)
#define SECONDS_PER_MIN         (60)
#define DCFDT_UNKNOW_TYPE       (0xFF)

#define DCFDT_FILENAME_YEAR_OFFSET      2000
#define ENABLE_DATE_VALUE_DEBUG  		0
#define DCFDT_DEBUG                 	0

//---------------------------------------------------------------------------------

typedef unsigned int DATE_TIME;

typedef struct sDATETIMEFILE{
	struct sDATETIMEFILE* pPrev;
	struct sDATETIMEFILE* pNext;

	DATE_TIME nDateTime;
#if RECORD_FILE_SIZE
//	UINT    nSizeInCluster;
#endif // RECORD_FILE_SIZE

//  Flag
    unsigned nSizeInCluster: 24;
	// File Type
	unsigned byFileType: 5;  //MOV, AVI, JPG, NMEA, ...
    unsigned bReadOnly : 1;
    unsigned bCharLock : 1;
#if FS_FORMAT_FREE_ENABLE    
    unsigned bHidden   : 1;
#endif
    unsigned bIsRearFileExist : 1;
#if ENABLE_DATE_VALUE_DEBUG //DEBUG
    UINT16  uwYear;
	UINT16  uwMonth;
	UINT16  uwDay;
	UINT16  uwHour;
	UINT16  uwMinute;
	UINT16  uwSecond;
#endif   //ENABLE_DATE_VALUE_DEBUG
} SDATETIMEFILE,*PSDATETIMEFILE;

//---------------------------------------------------------------------------------
typedef struct sDATETIMEDCFDB{

	// 定義區
	char szName[64];
	char szTemplate[64];
    char szTemplate_2nd[64];
	// Folder 路徑
	char  szFolderName[64];
    char  szFolderName_2nd[64];
    ULONG ulFilenameLen;
	// Free charater
	// Normal
	// G-lock
	// Manual
	
	// 屬性: 
    UINT32 uiFlag;
	// 變數區
	UINT uiCurrentIndex;
	// 檔案總數
	int ulTotalObjectNum;
    // rear cam file total num
    int ulTotalRearCamObjectNum;
	// 所有檔案大小總和
	UINT64 ullTotalObjectSize;
	// 最大檔案大小總和
	UINT32 ulMaxSizeThresholdInCluster;
    // 磁碟中不屬於DCF Objects的檔案
    UINT32 ulPayloadInCluster;

    ULONG  ulClusterSize;
	ULONG  ulTotalLockedObjects;
    UINT64 ullTotalLockedObjectSize;
	ULONG  ulTotalUnlockedObjects;
    UINT64 ullTotalUnlockedObjectSize;

    UINT64 ullUnlockedObjectNum[MAX_DCF_SUPPORT_FILE_TYPE];
    UINT64 ullUnlockedObjectSize[MAX_DCF_SUPPORT_FILE_TYPE];
    UINT64 ullLockedObjectNum[MAX_DCF_SUPPORT_FILE_TYPE];
    UINT64 ullLockedObjectSize[MAX_DCF_SUPPORT_FILE_TYPE];

    INT8   chAllowedFileType[MAX_DCF_SUPPORT_FILE_TYPE];

    AHC_BOOL bThreadConsiderPayload;

    // Disk Size in Clusters
    ULONG  ullDiskClusters;

	// max time
	// min time
    AHC_OS_SEMID smLock;

	struct sDATETIMEFILE* pFileHead;
    //Table to storage LockFile node Memory location 
    UINT32 ulCharLockFileTable[MAX_LOCK_FILE_NUM];
} SDATETIMEDCFDB,*PSDATETIMEDCFDB;

typedef enum{
  DCFDT_DB_FLAG_COUNT_PAYLOAD_INTO_TH = 0x01,
  DCFDT_DB_FLAG_HAS_NMEA_FILE         = 0x02
} DCF_DB_FLAG;

typedef enum{
  DCFDT_LOCKTABLE_LOCK,
  DCFDT_LOCKTABLE_UNLOCK
} DCFDT_LOCKTABLE_OP;

#if (GPS_RAW_FILE_ENABLE == 1)
#define DCFDT_DB0_FLAG                      (DCFDT_DB_FLAG_COUNT_PAYLOAD_INTO_TH|DCFDT_DB_FLAG_HAS_NMEA_FILE)
#define DCFDT_DB1_FLAG                      (DCFDT_DB_FLAG_HAS_NMEA_FILE)
#define DCFDT_DB2_FLAG                      (DCFDT_DB_FLAG_HAS_NMEA_FILE)
#define DCFDT_DB3_FLAG                      0
#else
#define DCFDT_DB0_FLAG                      DCFDT_DB_FLAG_COUNT_PAYLOAD_INTO_TH
#define DCFDT_DB1_FLAG                      0
#define DCFDT_DB2_FLAG                      0
#define DCFDT_DB3_FLAG                      0
#endif

#if DCF_DUAL_CAM_ENABLE

#define DCFDT_DB0_FOLDER                    "SD:\\Normal\\F"
#define DCFDT_DB0_REAR_FOLDER               "SD:\\Normal\\R"
#define DCFDT_SD_DB0_FOLDER_NAME_LEN		sizeof(DCFDT_DB0_FOLDER)
#define DCFDT_DB0_FILENAME_TEMPLATE         "FILE%02d%02d%02d-%02d%02d%02dF"
#define DCFDT_DB0_REAR_FILENAME_TEMPLATE    "FILE%02d%02d%02d-%02d%02d%02dR"
#define DCFDT_DB0_FILENAME_LEN              (4+6+1+6+4+1)

#define DCFDT_DB1_FOLDER                    "SD:\\Parking\\F"
#define DCFDT_DB1_REAR_FOLDER               "SD:\\Parking\\R"
#define DCFDT_SD_DB1_FOLDER_NAME_LEN		sizeof(DCFDT_DB1_FOLDER)
#define DCFDT_DB1_FILENAME_TEMPLATE         "PARK%02d%02d%02d-%02d%02d%02dF"
#define DCFDT_DB1_REAR_FILENAME_TEMPLATE    "PARK%02d%02d%02d-%02d%02d%02dR"
#define DCFDT_DB1_FILENAME_LEN              (4+6+1+6+4+1)

#define DCFDT_DB2_FOLDER                    "SD:\\Event\\F"
#define DCFDT_DB2_REAR_FOLDER               "SD:\\Event\\R"
#define DCFDT_SD_DB2_FOLDER_NAME_LEN		sizeof(DCFDT_DB2_FOLDER)
#define DCFDT_DB2_FILENAME_TEMPLATE         "EMER%02d%02d%02d-%02d%02d%02dF"
#define DCFDT_DB2_REAR_FILENAME_TEMPLATE    "EMER%02d%02d%02d-%02d%02d%02dR"
#define DCFDT_DB2_FILENAME_LEN              (4+6+1+6+4+1)

#define DCFDT_DB3_FOLDER                    "SD:\\Photo\\F"
#define DCFDT_DB3_REAR_FOLDER               "SD:\\Photo\\R"
#define DCFDT_SD_DB3_FOLDER_NAME_LEN		sizeof(DCFDT_DB3_FOLDER)
#define DCFDT_DB3_FILENAME_TEMPLATE         "IMG%02d%02d%02d-%02d%02d%02dF"
#define DCFDT_DB3_REAR_FILENAME_TEMPLATE    "IMG%02d%02d%02d-%02d%02d%02dR"
#define DCFDT_DB3_FILENAME_LEN              (3+6+1+6+4+1)

#define DCFDT_CHARLOCK_TEMPLATE             "LOCK%02d%02d%02d-%02d%02d%02dF"
#define DCFDT_CHARLOCK_REAR_TEMPLATE        "LOCK%02d%02d%02d-%02d%02d%02dR"
#else

#ifdef CAR_DV
#define DCFDT_DB0_FOLDER                    "SD:\\Normal"
#else
#define DCFDT_DB0_FOLDER                    "SD:\\Video"
#endif //endif CAR_DV
#define DCFDT_DB0_REAR_FOLDER               DCFDT_DB0_FOLDER
#define DCFDT_SD_DB0_FOLDER_NAME_LEN		sizeof(DCFDT_DB0_FOLDER)
#define DCFDT_DB0_FILENAME_TEMPLATE         "FILE%02d%02d%02d-%02d%02d%02d"
#define DCFDT_DB0_REAR_FILENAME_TEMPLATE    DCFDT_DB0_FILENAME_TEMPLATE
#define DCFDT_DB0_FILENAME_LEN              (4+6+1+6+4)

#define DCFDT_DB1_FOLDER                    "SD:\\Parking"
#define DCFDT_DB1_REAR_FOLDER               DCFDT_DB1_FOLDER
#define DCFDT_SD_DB1_FOLDER_NAME_LEN		sizeof(DCFDT_DB1_FOLDER)
#define DCFDT_DB1_FILENAME_TEMPLATE         "PARK%02d%02d%02d-%02d%02d%02d"
#define DCFDT_DB1_REAR_FILENAME_TEMPLATE    DCFDT_DB1_FILENAME_TEMPLATE
#define DCFDT_DB1_FILENAME_LEN              (4+6+1+6+4)

#define DCFDT_DB2_FOLDER                    "SD:\\Event"
#define DCFDT_DB2_REAR_FOLDER               DCFDT_DB2_FOLDER
#define DCFDT_SD_DB2_FOLDER_NAME_LEN		sizeof(DCFDT_DB2_FOLDER)
#define DCFDT_DB2_FILENAME_TEMPLATE         "EMER%02d%02d%02d-%02d%02d%02d"
#define DCFDT_DB2_REAR_FILENAME_TEMPLATE    DCFDT_DB2_FILENAME_TEMPLATE
#define DCFDT_DB2_FILENAME_LEN              (4+6+1+6+4)

#define DCFDT_DB3_FOLDER                    "SD:\\Photo"
#define DCFDT_DB3_REAR_FOLDER               DCFDT_DB3_FOLDER
#define DCFDT_SD_DB3_FOLDER_NAME_LEN		sizeof(DCFDT_DB3_FOLDER)
#define DCFDT_DB3_FILENAME_TEMPLATE         "IMG%02d%02d%02d-%02d%02d%02d"
#define DCFDT_DB3_REAR_FILENAME_TEMPLATE    DCFDT_DB3_FILENAME_TEMPLATE
#define DCFDT_DB3_FILENAME_LEN              (3+6+1+6+4)

#define DCFDT_CHARLOCK_TEMPLATE             "LOCK%02d%02d%02d-%02d%02d%02d"
#endif // endif DCF_DUAL_CAM_ENABLE

#ifdef CAR_DV
#define DCFDT_DB0_FOLDER_SD1                "SD1:\\Normal"
#else
#define DCFDT_DB0_FOLDER_SD1                "SD1:\\Video"
#endif
#define DCFDT_SD1_DB0_FOLDER_NAME_LEN		sizeof(DCFDT_DB0_FOLDER_SD1)
#define DCFDT_DB1_FOLDER_SD1                "SD1:\\Parking"
#define DCFDT_SD1_DB1_FOLDER_NAME_LEN		sizeof(DCFDT_DB1_FOLDER_SD1)
#define DCFDT_DB2_FOLDER_SD1                "SD1:\\Event"
#define DCFDT_SD1_DB2_FOLDER_NAME_LEN		sizeof(DCFDT_DB2_FOLDER_SD1)
#define DCFDT_DB3_FOLDER_SD1                "SD1:\\Photo"
#define DCFDT_SD1_DB3_FOLDER_NAME_LEN		sizeof(DCFDT_DB3_FOLDER_SD1)


#define DCFDT_NEMA_FILE_EXTENTION           "NMEA"

#define _T(X) 		X

#define DCFDT_DELETE_OLD_DUPLICATE          1

/*******************************************************************************
 *
 *   DEFINITIONS
 *
*******************************************************************************/

AHC_BOOL AHC_DCFDT_GetTotalFileCount(UINT32 *puiImgIndex);
AHC_BOOL AHC_DCFDT_GetTotalRearFileCount(UINT32 *puiImgIndex);
AHC_BOOL AHC_DCFDT_RenameFile(DCF_DB_TYPE sFromType, AHC_RTC_TIME* psRtcTimeOld, AHC_RTC_TIME* psRtcTimeNew);
AHC_BOOL AHC_DCFDT_RenameFilebyNode(PSDATETIMEDCFDB pSrcDb, PSDATETIMEFILE  psSrcNode, 
                                    char* szSrcFullPath   , char* szDstFullPath,
                                    DATE_TIME nDateTimeOld, DATE_TIME nDateTimeNew, 
                                    UINT32 *puiFolderNameLen, AHC_BOOL bIsRearCam);
void     AIHC_DCFDT_DumpDB(void);
AHC_BOOL AHC_DCFDT_SetCurrentIndex(UINT32 uiImgIndex);
AHC_BOOL AHC_DCFDT_GetCurrentIndex(UINT32 *puiImgIndex);
AHC_BOOL AHC_DCFDT_SearchNearestRearFileIndex(UINT32 uiIndex, UINT32 *puiRearFileIndex);
AHC_BOOL AHC_DCFDT_GetFilePathNamebyIndex(UINT32 uiIndex, char* FilePathName);
AHC_BOOL AHC_DCFDT_GetFileNamebyIndex(UINT32 uiIndex, char* FilePathName);
AHC_BOOL AHC_DCFDT_GetFileSizebyIndex(UINT32 uiIndex, UINT32* FileSize);
AHC_BOOL AHC_DCFDT_GetFileTypebyIndex(UINT32 uiIndex, UINT8 *FileType) ;
AHC_BOOL AHC_DCFDT_GetFileTimebyIndex(UINT32 uiIndex, AHC_RTC_TIME *pRtcTime);
AHC_BOOL AHC_DCFDT_IsReadOnlybyIndex(UINT32 uiIndex, AHC_BOOL *bReadOnly);
AHC_BOOL AHC_DCFDT_SetFreeChar(UINT8 byDir, UINT8 byOp, UINT8 byChars[]);
AHC_BOOL AHC_DCFDT_GetAllowedChar(INT8* pbyChar, UINT32 uiLength);
AHC_BOOL AHC_DCFDT_EnableDB(DCF_DB_TYPE sType, AHC_BOOL bEnable);
AHC_BOOL AHC_DCFDT_GetDBStatus(DCF_DB_TYPE sType);
AHC_BOOL AHC_DCFDT_SelectDB(DCF_DB_TYPE sType);
DCF_DB_TYPE AHC_DCFDT_GetDB(void);
PSDATETIMEDCFDB AIHC_DCFDT_GetDbByType(DCF_DB_TYPE sType);
AHC_BOOL AIHC_DCFDT_DbDeleteCyclicFilebySize(PSDATETIMEDCFDB pDB, UINT64 ullSize );
void AIHC_DCFDT_DbDeleteCyclicFilebyNum(PSDATETIMEDCFDB pDB, UINT32 ulNum );
AHC_BOOL AHC_DCFDT_FileOperation(char *pPathName, 
                                 char *pFileName, 
                                 UINT8 byOperation, 
                                 char *pDestPath, 
                                 char *pDestFileName);
AHC_BOOL AHC_DCFDT_MoveFile(DCF_DB_TYPE sFromType, DCF_DB_TYPE sToType, UINT16 uwDirKey, INT8 *pchFileName, AHC_BOOL bReadOnly);
UINT32 AHC_DCFDT_GetCyclicCluster(DCF_DB_TYPE sDB);
UINT32 AHC_DCFDT_GetFileCluster(DCF_DB_TYPE sDB);
UINT32 AHC_DCFDT_GetClusterTH(DCF_DB_TYPE sDB);
AHC_BOOL AHC_DCFDT_AdjustAllocation(DCF_STORAGE_ALLOC eType);
UINT64 	 AHC_DCFDT_GetFileTH(DCF_STORAGE_ALLOC sType);
AHC_BOOL AIHC_DCFDT_MountAllDB(UINT32 uiMaxMemSize);
AHC_BOOL AIHC_DCFDT_UnmountAllDB(void);
AHC_BOOL AIHC_DCFDT_PreAddFile(UINT16 uwDirKey, INT8 *pchFileName);
AHC_BOOL AIHC_DCFDT_PostAddFile(UINT16 uwDirKey, INT8 *pchFileName);
AHC_BOOL AHC_DCFDT_GetName(AHC_RTC_TIME* psRtcTime, INT8 *FileFullPath, INT8 *FileName, UINT8 *bCreateNewDir);
AHC_BOOL AHC_DCFDT_IsDBMounted(void);
AHC_BOOL AHC_DCFDT_RegisterCallback(MMP_ULONG ulCBType, void *pCallback);
AHC_BOOL AHC_DCFDT_DbUpdateLastFiles(PSDATETIMEDCFDB pDB);
AHC_BOOL AHC_DCFDT_UpdateLastFiles(DCF_DB_TYPE sDB);
void AHC_DCFDT_SetRearCamPathFlag(AHC_BOOL bSetRearCam);
void AHC_DCFDT_GetRearCamPathFlag(AHC_BOOL *bGetRearCam);
AHC_BOOL AHC_DCFDT_IsRearCamFileExistbyIndex(UINT32 uiIndex, AHC_BOOL *bFileExist);
AHC_BOOL AHC_DCFDT_RearIndexMappingToNormalIndex(UINT32 uiRearIndex, UINT32 *puiIndex);
AHC_BOOL AHC_DCFDT_NormalIndexMappingToRearIndex(UINT32 puiIndex, UINT32 *uiRearIndex);
#if (FS_FORMAT_FREE_ENABLE)
AHC_BOOL AHC_DCFDT_SearchAvailableFileSlot(char* szFilePathName);
#endif
AHC_BOOL AHC_DCFDT_TemplateCmp( char* pInput, char* pTemplate );
INT8* AHC_DCFDT_GetRootDirName(void);

#endif //_AHC_DTDCF_H_

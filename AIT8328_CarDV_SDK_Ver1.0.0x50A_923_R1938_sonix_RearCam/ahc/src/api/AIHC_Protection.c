/*===========================================================================
 * Include files
 *===========================================================================*/ 
 
#include "AHC_FS.h"
#include "AHC_General.h"
#include "AHC_Parameter.h"
#include "AHC_Message.h"
#include "AHC_Menu.h"
#include "AHC_Video.h"
#include "AIHC_Video.h"
#include "AHC_Parameter.h"

#include "AHC_Config_SDK.h"
#include "AHC_Version.h"
#include "AHC_Media.h"
#include "AHC_Warningmsg.h"
#include "AHC_Utility.h"
#include "AHC_UF.h"
#include "AIHC_DCF.h"
#include "AHC_DCF.h"
#include "includes_fw.h"
#include "ait_utility.h"

/*===========================================================================
 * Global varible
 *===========================================================================*/ 
static AHC_PROTECT_TYPE m_VRProtectType  = AHC_PROTECT_NONE;

#if (PROTECTION_FOR_C003_P000)
static UINT32 m_uiVRSetTime;
static UINT32 m_uiVRDoneTime;

static UINT32 m_uiVRPrevInterval  = 60;///< 60 s
static UINT32 m_uiVRNextInterval  = 60;///< 60 s
#endif

/*===========================================================================
 * Main body
 *===========================================================================*/ 

void AHC_Protect_SetType(AHC_PROTECT_TYPE Type) 
{
    m_VRProtectType = Type;
    
    #if (PROTECTION_FOR_C003_P000)
    if(Type == AHC_PROTECT_MOVE){
        AHC_OS_GetTime(&m_uiVRSetTime);
    }
    #endif
}

AHC_PROTECT_TYPE AHC_Protect_GetType(void) 
{
    return m_VRProtectType;    
}

AHC_BOOL AHC_Protect_SpellFileName(char* FilePathName, INT8* pchDirName, INT8* pchFileName)
{
    UINT32  uiFileNameLength;
    INT8    tmpDirName[MAX_FILE_NAME_SIZE];
    GetPathDirStr(tmpDirName,sizeof(tmpDirName),FilePathName);
    memcpy(pchDirName,tmpDirName,AHC_StrLen(tmpDirName));
    uiFileNameLength = AHC_StrLen(FilePathName) - AHC_StrLen(pchDirName) - 1;
    memcpy(pchFileName,FilePathName + AHC_StrLen(pchDirName) + 1,uiFileNameLength);
    
    return AHC_TRUE;
}

AHC_BOOL AHC_Protect_SetVRFile(AHC_PROTECTION_OP Op, AHC_PROTECT_TYPE Type)
{
    #define MAX_NAME_LENGTH (32)
    
    UINT32          	uiMaxObjNumber;    
    char     			FilePathName[MAX_FILE_NAME_SIZE];
	UINT16              DirKey;
	
    INT8 				OldDirName[MAX_NAME_LENGTH];
    INT8 				OldFileName[MAX_NAME_LENGTH];
    
	AHC_UF_SetFreeChar(0, DCF_SET_ALLOWED, (UINT8*)SEARCH_MOVIE);
 		
	AHC_UF_GetTotalFileCount(&uiMaxObjNumber);

    #if (PROTECTION_FOR_C003_P000)
    if(Type == AHC_PROTECT_MOVE){
        UINT32 uiVRInterval;
        UINT32 uiDelta;

        AHC_OS_GetTime(&m_uiVRDoneTime);
        uiVRInterval = AHC_GetVRTimeLimit();
        
        if(m_uiVRDoneTime > m_uiVRSetTime){
            
            uiDelta = m_uiVRDoneTime - m_uiVRSetTime;
            uiDelta = uiDelta / 1000;
            
        }
        else{
            uiDelta = (0xFFFFFFFF - m_uiVRDoneTime) + m_uiVRSetTime;
            uiDelta = uiDelta / 1000;
        }
        
        if((uiVRInterval - uiDelta) <= m_uiVRPrevInterval){
            Op = AHC_PROTECTION_PRE_FILE|AHC_PROTECTION_CUR_FILE;
        }
        else if(uiDelta <= m_uiVRNextInterval) {
            m_VRProtectType = AHC_PROTECT_MOVE|AHC_PROTECT_NEXT;
            Type = AHC_PROTECT_MOVE|AHC_PROTECT_NEXT;
        }
        
    }
    else if (Type == AHC_PROTECT_NEXT) {
        m_VRProtectType = AHC_PROTECT_MOVE;
        Type = AHC_PROTECT_MOVE;
    }
    #endif
    
	if(uiMaxObjNumber == 0){
		return AHC_FALSE;
	}else if(uiMaxObjNumber == 1){
	
		goto FINAL;
    }
    
 	if((Op & AHC_PROTECTION_PRE_FILE) != 0)
 	{
 	    UINT32 uiObjIndex;

        uiObjIndex = uiMaxObjNumber - 2;
 	    MEMSET(FilePathName, 0, sizeof(FilePathName));
		AHC_UF_GetFilePathNamebyIndex(uiObjIndex, FilePathName);
		AHC_UF_GetDirKeybyIndex(uiObjIndex, &DirKey);
			
    	printc("PreFileName: %s \n", FilePathName);

        MEMSET(OldDirName, 0, sizeof(OldDirName));
        MEMSET(OldFileName, 0, sizeof(OldFileName));
        
        AHC_Protect_SpellFileName(FilePathName, OldDirName, OldFileName);
        
        #if (PROTECT_FILE_TYPE == PROTECT_FILE_MOVE_SUBDB)///< #if (PROTECT_FILE_TYPE)

        if((Type & AHC_PROTECT_MENU) != 0){
		    AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL, NULL);
		}    
		
    	if(((Type & AHC_PROTECT_G) != 0) || ((Type & AHC_PROTECT_MOVE) != 0)){

            AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_MOVE_SUBDB, NULL, NULL);            
        }    

		#elif (PROTECT_FILE_TYPE == PROTECT_FILE_RENAME) ///< #if (PROTECT_FILE_TYPE)
        {
        INT8    NewFileName[MAX_NAME_LENGTH];

        MEMSET(NewFileName, 0, sizeof(NewFileName));

    	STRCPY(NewFileName, OldFileName);

        if((Type & AHC_PROTECT_MENU) != 0){
		    AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
		}    
		
    	if((Type & AHC_PROTECT_G) != 0){
            MEMCPY(NewFileName, DCF_CHARLOCK_PATTERN, STRLEN(DCF_CHARLOCK_PATTERN));

            AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_CHAR_LOCK, NULL, NULL);
        }    
        #if (LIMIT_MAX_LOCK_FILE_NUM && MAX_LOCK_FILE_ACT!=LOCK_FILE_STOP)
        AHC_UF_UpdateLockFileTable(uiObjIndex,NULL);
        #endif
		printc("NewFileName: %s \n", NewFileName);
		}
		
		#elif (PROTECT_FILE_TYPE == PROTECT_FILE_MOVE_DB) ///< #if (PROTECT_FILE_TYPE)
        if(!(AHC_UF_MoveFile(AHC_UF_GetDB(), DCF_DB_TYPE_3RD_DB, DirKey, OldFileName, AHC_FALSE)))
        {
            printc("MoveFile to DB %d failed\r\n",DCF_DB_TYPE_3RD_DB);
            printc("Replace it by ReadOnly Operation\r\n");
            AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
        }
		#else///< #if (PROTECT_FILE_TYPE)
        
        AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
        
		#endif///< #if (PROTECT_FILE_TYPE)
	}

FINAL:

	if((Op & AHC_PROTECTION_CUR_FILE) != 0)  
	{
	    UINT32 uiObjIndex;
		AHC_UF_GetTotalFileCount(&uiMaxObjNumber);
        uiObjIndex = uiMaxObjNumber-1;
        
		MEMSET(FilePathName, 0, sizeof(FilePathName));
		AHC_UF_GetFilePathNamebyIndex(uiObjIndex, FilePathName);
		AHC_UF_GetDirKeybyIndex(uiObjIndex, &DirKey);
		printc("CurFileName: %s \n", FilePathName);

		MEMSET(OldDirName, 0, sizeof(OldDirName));
		MEMSET(OldFileName, 0, sizeof(OldFileName));
		
		 
        AHC_Protect_SpellFileName(FilePathName, OldDirName, OldFileName);
        
        #if (PROTECT_FILE_TYPE == PROTECT_FILE_MOVE_SUBDB)///< #if (PROTECT_FILE_TYPE)

        if((Type & AHC_PROTECT_MENU) != 0) {
		    AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
		}
		
        if(((Type & AHC_PROTECT_G) != 0) || ((Type & AHC_PROTECT_MOVE) != 0)) {
            AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_MOVE_SUBDB, NULL, NULL);
        }

		#elif (PROTECT_FILE_TYPE == PROTECT_FILE_RENAME)///< #if (PROTECT_FILE_TYPE)
		{
        INT8    NewFileName[MAX_NAME_LENGTH];

        MEMSET(NewFileName, 0, sizeof(NewFileName));
        
        STRCPY(NewFileName, OldFileName);
        
        if((Type & AHC_PROTECT_MENU) != 0) {
		    AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
		}
		
        if((Type & AHC_PROTECT_G) != 0) {
        
            MEMCPY(NewFileName, DCF_CHARLOCK_PATTERN, STRLEN(DCF_CHARLOCK_PATTERN));

			#if (USE_INFO_FILE)
            STRCPY(AHC_InfoLog()->DCFCurVideoFileName, NewFileName);
			#endif            

            AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_CHAR_LOCK, NULL, NULL);
        }
		#if (LIMIT_MAX_LOCK_FILE_NUM && MAX_LOCK_FILE_ACT!=LOCK_FILE_STOP)
        AHC_UF_UpdateLockFileTable(uiObjIndex,NULL);
        #endif
		printc("NewFileName: %s \n", NewFileName);
		}
		
		#elif (PROTECT_FILE_TYPE == PROTECT_FILE_MOVE_DB) ///< #if (PROTECT_FILE_TYPE)
		if(!(AHC_UF_MoveFile(AHC_UF_GetDB(), DCF_DB_TYPE_3RD_DB, DirKey, OldFileName, AHC_FALSE)))
        {
            printc("MoveFile to DB %d failed\r\n",DCF_DB_TYPE_3RD_DB);
            printc("Replace it by ReadOnly Operation\r\n");
            AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
        }
		#else///< #if (PROTECT_FILE_TYPE)
        
        AHC_UF_FileOperation((UINT8*)OldDirName,(UINT8*)OldFileName, DCF_FILE_READ_ONLY, NULL,NULL);
        
		#endif///< #if (PROTECT_FILE_TYPE)
	}
	return AHC_TRUE;	
}

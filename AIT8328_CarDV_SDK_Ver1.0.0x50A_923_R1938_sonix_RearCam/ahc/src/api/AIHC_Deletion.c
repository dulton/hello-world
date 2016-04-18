#if 0
void __HEADER__(){}
#endif
/*===========================================================================
 * Include file 
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
#include "ait_utility.h"

#include "StateVideoFunc.h"
/*===========================================================================
 * Main body
 *===========================================================================*/

#define USE_DIR_BUFFER (1)

#if (USE_DIR_BUFFER)
#define VIDEO_DIR_BUFFER_SIZE    (500*1024*1024)
#define STILL_DIR_BUFFER_SIZE    (10*1024*1024)
#define MAX_NMEA_TOTAL_SIZE      (20*1024*1024)  
#endif


#if (DELETION_STYLE == DELETION_STYLE_BASIC)
/*
delete current DB
*/
static AHC_BOOL AHC_Deletion_Romove_Basic(AHC_BOOL bFirstTime)
{
 	AHC_BOOL	ahc_ret = AHC_TRUE;
    UINT64      ulFreeSpace 	= 0;
	UINT32 		ulAudBitRate 	= 128000;//128K
    UINT32      ulVidBitRate	= 0;
	UINT32		ulTimelimit		= 0;
	UINT64      ulSpaceNeeded 	= 0;
	UINT32 		ulVideoReservedSize;
	
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_MOVIE);

	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 
		
	ulTimelimit = AHC_GetVRTimeLimit();
		
	if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
	{
		ulSpaceNeeded = 0x24000000;/* 576MB */
	}
	else
	{
		ulSpaceNeeded = (( (((UINT64)(ulVidBitRate + ulAudBitRate)>>3)*(UINT64)ulTimelimit) >>3) *9);
	
		if(bFirstTime)
			ulSpaceNeeded *= 2;
	}
	
	VideoFunc_GetFreeSpace(&ulFreeSpace);
	
	AIHC_GetMovieConfig( AHC_VIDEO_RESERVED_SIZE, &ulVideoReservedSize );
	
	if( ulFreeSpace >= ulSpaceNeeded )
	{
		printc("The Free space needed %d KB\r\n",(ulSpaceNeeded>>10));
		return AHC_TRUE;
	}else if((ulFreeSpace < ulSpaceNeeded) && (ulFreeSpace < ulVideoReservedSize))
	{
		if(MenuSettingConfig()->uiCyclicRecord == MOVIE_CYCLIC_RECORD_OFF)
		return AHC_FALSE;
	}else if((ulFreeSpace < ulSpaceNeeded) && (ulFreeSpace > ulVideoReservedSize))
	{
		if(MenuSettingConfig()->uiCyclicRecord == MOVIE_CYCLIC_RECORD_OFF)
		return AHC_TRUE;
	}else
	{
		printc("Exception\r\n");
	}
	
	#if (AHC_DCF_BACKGROUND == 1)
	AIHC_DCF_SetupNeededSpace(ulSpaceNeeded);
	#endif
	
	ulSpaceNeeded -= ulFreeSpace;
	ahc_ret = AHC_UF_DeleteCyclicFilebySize(ulSpaceNeeded);	
	
	return ahc_ret;
}
#elif (DELETION_STYLE == DELETION_STYLE_C005)
/**
1st DB = 3/4 * memory space.
2nd DB = 1/4 * memory space.
*/
static AHC_BOOL AHC_Deletion_Romove_C005(AHC_BOOL bFirstTime)
{
 	AHC_BOOL	uiResult = AHC_TRUE;
    UINT64      ulFreeSpace 	= 0;
	UINT64      ulSpaceNeeded 	= 0;
    DCF_DB_TYPE sCurDB;
    UINT32      i;
	UINT32      ulSpaceNeededInClusters;    

    #define DELETION_DEBUG (1)

	#if(defined(DEVICE_GPIO_2NDSD_PLUG))

	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
	if ( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState() )
	#endif
						
	#else//DEVICE_GPIO_2NDSD_PLUG
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#endif//DEVICE_GPIO_2NDSD_PLUG
    {
		AHC_UF_UpdateLastFiles(DCF_DB_TYPE_1ST_DB);
	}
	
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_PHOTO_MOVIE);

    {
    	UINT32 		ulAudBitRate 	= 128000;//128K
        UINT32      ulVidBitRate	= 0;
    	UINT32		ulTimelimit		= 0;
    	
    	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 
    		
    	ulTimelimit = AHC_GetVRTimeLimit();
    		
    	if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
    	{
    		ulSpaceNeeded = 0x24000000;/* 576MB */
    	}
    	else
    	{
    		ulSpaceNeeded = (( (((UINT64)(ulVidBitRate + ulAudBitRate)>>3)*(UINT64)ulTimelimit) >>3) *9);
    	
    		if(bFirstTime) {
    			ulSpaceNeeded *= 2;
    		}
			else {
				ulSpaceNeeded = (ulSpaceNeeded *12)/10;
			}	
    	}
    }
    
    sCurDB = AHC_UF_GetDB();
    
    /**
    The original folder has enough space to be deleted.
    */
#if (USE_DIR_BUFFER)

    if( sCurDB == DCF_DB_TYPE_4TH_DB )
    {
        ulSpaceNeededInClusters = STILL_DIR_BUFFER_SIZE /AHC_UF_GetClusterSize(sCurDB); 
    }
    else
    {
        ulSpaceNeededInClusters = ulSpaceNeeded /AHC_UF_GetClusterSize(sCurDB);
    }

    ulSpaceNeeded = ulSpaceNeededInClusters * AHC_UF_GetClusterSize(sCurDB);

    if( AHC_UF_GetFileCluster(sCurDB)+ ulSpaceNeededInClusters > AHC_UF_GetClusterTH(sCurDB) )
#else
    if(AHC_UF_GetFileCluster(sCurDB) > AHC_UF_GetClusterTH(sCurDB))
#endif
	{
        UINT32 uiSpaceToDelete;
        #if DELETION_DEBUG
        printc("Deletion 0x1 DB%d space needed %d\n",sCurDB, ulSpaceNeededInClusters);
        #endif
        
        AHC_UF_SelectDB(sCurDB);

        if( AHC_UF_GetFileCluster(sCurDB) > AHC_UF_GetClusterTH(sCurDB) )
        {
            // file size is over threshold
            ulSpaceNeededInClusters += ( AHC_UF_GetFileCluster(sCurDB) - AHC_UF_GetClusterTH(sCurDB) );
        }
        else
        {
            ulSpaceNeededInClusters -=  (AHC_UF_GetClusterTH(sCurDB) - AHC_UF_GetFileCluster(sCurDB));
        }

        uiSpaceToDelete = ulSpaceNeededInClusters * AHC_UF_GetClusterSize(sCurDB);

        if(AHC_UF_DeleteCyclicFilebySize(uiSpaceToDelete) == AHC_TRUE){
            uiResult = AHC_TRUE;
            goto Final;
        }
    }
    
    for(i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++){

	    if(AHC_UF_GetFileCluster(i) > AHC_UF_GetClusterTH(i))
		{
        
            #if DELETION_DEBUG
            printc("Deletion 0x2 : %d \n", i);
            #endif

            VideoFunc_GetFreeSpace(&ulFreeSpace);

            AHC_UF_SelectDB(i);

            if( ulFreeSpace >= ulSpaceNeeded ){
                    goto Final;            
            }else{
            
                if(AHC_UF_DeleteCyclicFilebySize(ulSpaceNeeded) == AHC_TRUE){
                    uiResult = AHC_TRUE;
                    goto Final;
                }
            }
        
        }
        
    }
   
    for(i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++){

        {
            VideoFunc_GetFreeSpace(&ulFreeSpace);

#if DELETION_DEBUG
            printc("Deletion 0x3 : %d %d %d \n", i, ulSpaceNeeded, ulFreeSpace);
#endif
            
            AHC_UF_SelectDB(i);
            
            if( ulFreeSpace >= ulSpaceNeeded ){
                    goto Final;            
            }else{
            
                if(AHC_UF_DeleteCyclicFilebySize(ulSpaceNeeded) == AHC_TRUE){
                    uiResult = AHC_TRUE;
                    goto Final;
                }
            }
        
        }
        
    }
	
    {

    VideoFunc_GetFreeSpace(&ulFreeSpace);
    
	if( ulFreeSpace >= ulSpaceNeeded )
	{
        #if DELETION_DEBUG
        printc("Deletion 0x4 \n");
        #endif
        uiResult = AHC_TRUE;
        goto Final;
        
    }else{
    
        #if DELETION_DEBUG
        printc("Deletion 0x5 \n");
        #endif
    }
    
	}
	
Final:

    
    AHC_UF_SelectDB(sCurDB);
	
	return uiResult;
}

static AHC_BOOL AHC_Deletion_Romove_By_FileNum_Before(AHC_BOOL bFirstTime)
{
	
 	AHC_BOOL	uiResult = AHC_TRUE;
    UINT64      ulFreeSpace 	= 0;
	UINT64      ulSpaceNeeded 	= 0;
    DCF_DB_TYPE sCurDB;
    UINT32      i, j;
	UINT32      ulSpaceNeededInClusters;
	UINT32		FileCount[4];
	AHC_BOOL	bNeedtoDelete;
	UINT8		ubDeleDB;

    #define DELETION_DEBUG (1)

	#if(defined(DEVICE_GPIO_2NDSD_PLUG))

	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
	if ( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState() )
	#endif

	#else//DEVICE_GPIO_2NDSD_PLUG
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#endif//DEVICE_GPIO_2NDSD_PLUG
    {
		AHC_UF_UpdateLastFiles(DCF_DB_TYPE_1ST_DB);
	}
	
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_PHOTO_MOVIE);
 
    sCurDB = AHC_UF_GetDB();
    
    /**
    Space needed if the file count is not fill but no space, delete normal DB
    */
    {
    	UINT32 		ulAudBitRate 	= 128000;//128K
        UINT32      ulVidBitRate	= 0;
    	UINT32		ulTimelimit		= 0;
    	
    	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 
    		
    	ulTimelimit = AHC_GetVRTimeLimit();
    		
    	if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
    	{
    		ulSpaceNeeded = 0x24000000;/* 576MB */
    	}
    	else
    	{
    		ulSpaceNeeded = (( (((UINT64)(ulVidBitRate + ulAudBitRate)>>3)*(UINT64)ulTimelimit) >>3) *9);
    	
    		if(bFirstTime) {
    			ulSpaceNeeded *= 2;
    		}
			else {
				ulSpaceNeeded = (ulSpaceNeeded *12)/10;
			}	
    	}
    }
    
    /**
    Get File count of each DB,
    */	
	{
		for(i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++)
		{
			AHC_UF_SelectDB(i);
			AHC_UF_GetTotalFileCount(&FileCount[i]);
		}
		
	}
    
    /**
    Check File counts of each DB, Skip normal dir
    */    
    {
        UINT32 uiDeleteNum = 0;
		UINT8  ubAdditionalFile = 0;
		UINT8  ubCheckThreshold = MMP_FALSE;
    	for(i = DCF_DB_TYPE_2ND_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++)
    	{
    		
    		if(i == DCF_DB_TYPE_3RD_DB)
    		{
    			if(FileCount[i] >= AHC_UF_GetFileTH(i))
    			ubCheckThreshold = MMP_TRUE;
    			else
    			ubCheckThreshold = MMP_FALSE;
    		}else
    		{
	    		if(FileCount[i] > AHC_UF_GetFileTH(i))
	    		ubCheckThreshold = MMP_TRUE;
	    		else
	    		ubCheckThreshold = MMP_FALSE;
    		}
    		if(ubCheckThreshold)
	    		{
	    		    if(i == DCF_DB_TYPE_4TH_DB)
	    		    ubAdditionalFile = 0;
	    		    else if(i == DCF_DB_TYPE_3RD_DB)
	    		    ubAdditionalFile = 1;
	    		    else
	    		    ubAdditionalFile = 0;
	    		    uiDeleteNum  = FileCount[i] - AHC_UF_GetFileTH(i) + ubAdditionalFile;
	    		    printc("FileCount[%d] %d\r\n",i,FileCount[i]);
	                printc("AHC_UF_GetFileTH[%d] %d\r\n",i,AHC_UF_GetFileTH(i));
		    		AHC_UF_SelectDB(i);

			        if(AHC_UF_DeleteFilebyNum(uiDeleteNum) == AHC_TRUE){
			            uiResult = AHC_TRUE;
			        }
	    		}
    	}
    }
      
    /**
    Check free space, and detete from DB1
    */
    {
    	 	VideoFunc_GetFreeSpace(&ulFreeSpace);

	        AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);

	        if( ulFreeSpace >= ulSpaceNeeded ){
	                goto Final;            
	        }else{
	        
	            if(AHC_UF_DeleteCyclicFilebySize(ulSpaceNeeded) == AHC_TRUE){
	                uiResult = AHC_TRUE;
	                goto Final;
	            }
	        }
    }  
    
Final:

    
    AHC_UF_SelectDB(sCurDB);
	
	return uiResult;

}

static AHC_BOOL AHC_Deletion_Romove_By_FileNum(AHC_BOOL bFirstTime)
{
 	AHC_BOOL	uiResult = AHC_TRUE;
    UINT64      ulFreeSpace 	= 0;
	UINT64      ulSpaceNeeded 	= 0;
    DCF_DB_TYPE sCurDB;
    UINT32      i, j;
	UINT32		FileCount[4];
	AHC_BOOL	bNeedtoDelete;
	UINT8		ubDeleDB;

    #define DELETION_DEBUG (1)

	#if(defined(DEVICE_GPIO_2NDSD_PLUG))

	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
	if ( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState() )
	#endif

	#else//DEVICE_GPIO_2NDSD_PLUG
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#endif//DEVICE_GPIO_2NDSD_PLUG
    {
		AHC_UF_UpdateLastFiles(DCF_DB_TYPE_1ST_DB);
	}
	
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_PHOTO_MOVIE);
 
    sCurDB = AHC_UF_GetDB();
    
    /**
    Space needed if the file count is not fill but no space, delete normal DB
    */
    {
    	UINT32 		ulAudBitRate 	= 128000;//128K
        UINT32      ulVidBitRate	= 0;
    	UINT32		ulTimelimit		= 0;
    	
    	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 
    		
    	ulTimelimit = AHC_GetVRTimeLimit();
    		
    	if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
    	{
    		ulSpaceNeeded = 0x24000000;/* 576MB */
    	}
    	else
    	{
    		ulSpaceNeeded = (( (((UINT64)(ulVidBitRate + ulAudBitRate)>>3)*(UINT64)ulTimelimit) >>3) *9);
    	
    		if(bFirstTime) {
    			ulSpaceNeeded *= 2;
    		}
			else {
				ulSpaceNeeded = (ulSpaceNeeded *12)/10;
			}	
    	}
    }
    
    /**
    Get File count of each DB,
    */	
	{
		for(i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++)
		{
			AHC_UF_SelectDB(i);
			AHC_UF_GetTotalFileCount(&FileCount[i]);
		}
		
	}
    
    /**
    Check File counts of each DB, Skip normal dir
    */    
    {
        UINT32 uiDeleteNum = 0;
		UINT8  ubAdditionalFile = 0;
		
    	for(i = DCF_DB_TYPE_2ND_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++)
    	{
    		if(FileCount[i] > AHC_UF_GetFileTH(i))
    		{
    		    if(i == DCF_DB_TYPE_4TH_DB)
    		    ubAdditionalFile = 1;
    		    else
    		    ubAdditionalFile = 0;
    		    uiDeleteNum  = FileCount[i] - AHC_UF_GetFileTH(i) + ubAdditionalFile;
    		    //printc("FileCount[%d] %d\r\n",i,FileCount[i]);
                //printc("AHC_UF_GetFileTH[%d] %d\r\n",i,AHC_UF_GetFileTH(i));
	    		AHC_UF_SelectDB(i);

		        if(AHC_UF_DeleteFilebyNum(uiDeleteNum) == AHC_TRUE){
		            uiResult = AHC_TRUE;
		        }
    		}
    	}
    }
      
    /**
    Check free space, and detete from DB1
    */
    {
    	 	VideoFunc_GetFreeSpace(&ulFreeSpace);

	        AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);

	        if( ulFreeSpace >= ulSpaceNeeded ){
	                goto Final;            
	        }else{
	        
	            if(AHC_UF_DeleteCyclicFilebySize(ulSpaceNeeded) == AHC_TRUE){
	                uiResult = AHC_TRUE;
	                goto Final;
	            }
	        }
    }  
    
Final:

    
    AHC_UF_SelectDB(sCurDB);
	
	return uiResult;
}

#elif (DELETION_STYLE == DELETION_CARDV)
static AHC_BOOL AHC_Deletion_Remove_CarDV(AHC_BOOL bFirstTime)
{
	#define CHECK_CROSS_LINK	(0)

 	AHC_BOOL			ahc_ret = AHC_TRUE;
   	UINT32				MaxObjIdx		= 0;
    UINT32      		CurObjIdx		= 0;

	char                FilePathName[MAX_FILE_NAME_SIZE];
	AHC_BOOL            bReadOnly;
	AHC_BOOL            bProtectFile;
	UINT32              FileSize;
	
    UINT32      		StrLen;
    INT8       			DirName[32],FileName[32];
    UINT64      		ulFreeSpace 	= 0;
	UINT32 				ulAudBitRate 	= 128000;//128K
    UINT32      		ulVidBitRate	= 0;
	UINT32				ulTimelimit		= 0;
	UINT64      		ulSpaceNeeded 	= 0;
	UINT32				InitObjIdx 		= 0;
	INT16               DirLenCnt       = 0;
#if (CHECK_CROSS_LINK)	
	UINT64				ulFreeSpace1	= 0;
	UINT32				ulCluseterSize	= 0;
	AHC_FS_DISK_INFO 	volinfo;
		
	AHC_FS_GetVolumeInfo("SD:\\", strlen("SD:\\"), &volinfo);
	
	ulCluseterSize = (volinfo.usBytesPerSector)*(volinfo.usSecPerCluster);	
#endif	
	
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_MOVIE);

	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 
		
	ulTimelimit = AHC_GetVRTimeLimit();
		
	if(ulTimelimit==NON_CYCLING_TIME_LIMIT)
	{
		ulSpaceNeeded = 0x24000000;/* 576MB */
	}
	else
	{
        AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, ulTimelimit, &ulSpaceNeeded);
	
		if(bFirstTime)
			ulSpaceNeeded *= 2;
	}

    printc("Min free space %dMB is need\r\n", ulSpaceNeeded >> 20);

	#if (!CHECK_CROSS_LINK)
	VideoFunc_GetFreeSpace(&ulFreeSpace);
	#endif	
		
	do {
	
		#if (CHECK_CROSS_LINK)
		VideoFunc_GetFreeSpace(&ulFreeSpace);
		#endif
		
        printc("SD ulFreeSpace = %d KB\r\n",(ulFreeSpace>>10));
		
		if( ulFreeSpace >= ulSpaceNeeded )
		{
			printc(FG_BLUE("FreeSpace is Enough [SpaceNeeded %d KB]\r\n"),(ulSpaceNeeded>>10));
			return AHC_TRUE;
		}
		
	   	AHC_UF_GetTotalFileCount(&MaxObjIdx);

		if(MaxObjIdx == 0)
		{
			printc(FG_RED("No More DCF File for Delete!\r\n"));
			return AHC_FALSE;
		}
		
		if(InitObjIdx>=MaxObjIdx)
		{
			printc(FG_RED("All %d File Can't Delete!!\r\n"), MaxObjIdx);
			return AHC_FALSE;
		}
		
		AHC_UF_SetCurrentIndex(InitObjIdx); 
		AHC_UF_GetCurrentIndex(&CurObjIdx);

		MEMSET(FilePathName, 0, sizeof(FilePathName));
		if(AHC_FALSE == AHC_UF_GetFilePathNamebyIndex(CurObjIdx, FilePathName)) 
		{
			printc(FG_RED("AHC_UF_GetFilePathNamebyIndex Error\r\n"));
			return AHC_FALSE;
		}
		if(AHC_FALSE == AHC_UF_IsReadOnlybyIndex(CurObjIdx, &bReadOnly)) 
		{
			printc(FG_RED("AHC_UF_IsReadOnlybyIndex Error\r\n"));
			return AHC_FALSE;
		}
		if(AHC_FALSE == AHC_UF_GetFileSizebyIndex(CurObjIdx, &FileSize)) 
		{
			printc(FG_RED("AHC_UF_GetFileSizebyIndex Error\r\n"));
			return AHC_FALSE;
		}
		if(AHC_FALSE == AHC_UF_IsCharLockbyIndex(CurObjIdx, &bProtectFile)) 
		{
			printc(FG_RED("AHC_UF_IsReadOnlybyIndex Error\r\n"));
			return AHC_FALSE;
		}

		if(bReadOnly || bProtectFile)//Read Only File or Protect file
		{
			InitObjIdx++;
			continue;
		}
		
		StrLen = strlen(FilePathName);
	    DirLenCnt = StrLen;
		
	    for(DirLenCnt=(StrLen-1);DirLenCnt>=0;)
	    {
	        if(FilePathName[DirLenCnt] == '\\')
	       	{
				break;
	       	}
		    DirLenCnt--;
	    }

		MEMSET(DirName, 0, sizeof(DirName));
		MEMCPY(DirName, FilePathName, DirLenCnt);

		MEMSET(FileName, 0, sizeof(FileName));
		MEMCPY(FileName, (INT8 *) &FilePathName[DirLenCnt + 1], StrLen - DirLenCnt - 1);

		printc("Delete Min Key File %s\\%s \r\n",DirName, FileName);

		ahc_ret = AHC_UF_FileOperation((UINT8*)DirName,(UINT8*)FileName, DCF_FILE_DELETE, NULL,NULL);

	    if(ahc_ret==AHC_FALSE)
	    {
	    	printc(FG_RED("AHC_UF_FileOperation Delete Error\r\n"));
	    	return AHC_FALSE;
	    }
	    else
	    {
	    	#if (CHECK_CROSS_LINK)
			VideoFunc_GetFreeSpace(&ulFreeSpace1);
			
			if( ((ulFreeSpace + FileSize - ulCluseterSize) < ulFreeSpace1) &&
				((ulFreeSpace + FileSize + ulCluseterSize) > ulFreeSpace1) )
				;//Check Pass
			else
			{	
				printc("FS Cross Link!!!!!\r\n");
				AHC_WMSG_Draw(AHC_TRUE, WMSG_FORMAT_SD_CARD, 3);
				return AHC_FALSE;
			}
			#else
	    	ulFreeSpace += FileSize;
	    	#endif
	    }
	    
    } while(1);
    
	return ahc_ret;
}

#endif

AHC_BOOL AHC_Deletion_Confirm(DCF_DB_TYPE sDB, UINT32 uiTime)
{

    UINT64      ulFreeSpace 	= 0;
	UINT64      ulSpaceNeeded 	= 0;
	DCF_DB_TYPE sCurDB;
    UINT32 		ulAudBitRate 	= 128000;//128K
    UINT32      ulVidBitRate	= 0;
	UINT32      ulSpaceNeededInClusters;

    #define DELETION_DEBUG (1)
    
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_PHOTO_MOVIE);
	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 

	AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, uiTime, &ulSpaceNeeded);

	sCurDB = AHC_UF_GetDB();
	AHC_UF_SelectDB(sDB);
	
    #if(defined(DEVICE_GPIO_2NDSD_PLUG))

	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
	if ( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState() )
	#endif
						
	#else//DEVICE_GPIO_2NDSD_PLUG
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#endif//DEVICE_GPIO_2NDSD_PLUG
    {
		AHC_UF_UpdateLastFiles(sDB);
	}
	
    /**
    The original folder has enough space to be deleted.
    */
#if (USE_DIR_BUFFER)
    
    if( sDB == DCF_DB_TYPE_4TH_DB )
    {
        ulSpaceNeededInClusters = STILL_DIR_BUFFER_SIZE /AHC_UF_GetClusterSize(sDB); 
    }
    else
    {
        ulSpaceNeededInClusters = ulSpaceNeeded /AHC_UF_GetClusterSize(sDB);
    }

    ulSpaceNeeded = ulSpaceNeededInClusters * AHC_UF_GetClusterSize(sDB);

    if( AHC_UF_GetFileCluster(sDB)+ ulSpaceNeededInClusters > AHC_UF_GetClusterTH(sDB) )
#else

    if(AHC_UF_GetFileCluster(sDB) > AHC_UF_GetClusterTH(sDB))
#endif		
	{
	    AHC_UF_SelectDB(sCurDB);
        return AHC_FALSE;        
    }

    VideoFunc_GetFreeSpace(&ulFreeSpace);

	AHC_UF_SelectDB(sCurDB);
	
	if( ulFreeSpace >= ulSpaceNeeded )
	{
        return AHC_TRUE;
        
    }else{
        return AHC_FALSE;
    }

}

AHC_BOOL AHC_Deletion_ConfirmByNum(DCF_DB_TYPE sDB, UINT32 uiTime)
{
	UINT64      ulFreeSpace 	= 0;
	UINT64      ulSpaceNeeded 	= 0;
	DCF_DB_TYPE sCurDB;
    UINT32 		ulAudBitRate 	= 128000;//128K
    UINT32      ulVidBitRate	= 0;
  	UINT32		FileCount[4];
  	  
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_PHOTO_MOVIE);

	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 

	AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, uiTime, &ulSpaceNeeded);
	
	sCurDB = AHC_UF_GetDB();
	
	
    #if(defined(DEVICE_GPIO_2NDSD_PLUG))

	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_MASTER_SLAVE)
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#elif(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
	if ( (AHC_IsSDInserted() || AHC_Is2ndSDInserted()) && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState() )
	#endif
						
	#else//DEVICE_GPIO_2NDSD_PLUG
	if(AHC_IsSDInserted() && AHC_GetMountStorageMediaStatus() && AHC_SDMMC_GetMountState()) 
	#endif//DEVICE_GPIO_2NDSD_PLUG
    {
		AHC_UF_UpdateLastFiles(sDB);
	}
	
    AHC_UF_SelectDB(sDB);
	AHC_UF_GetTotalFileCount(&FileCount[sDB]);
    
    AHC_UF_SelectDB(sCurDB);
    
    if(sDB == DCF_DB_TYPE_3RD_DB)
    {
    	if(FileCount[DCF_DB_TYPE_3RD_DB] >= AHC_UF_GetFileTH(DCF_DB_TYPE_3RD_DB))
    	{
    		printc("Emer_File count is over threshold\r\n");
	   		return AHC_FALSE;
    	}
    }
       
    if(FileCount[sDB] > AHC_UF_GetFileTH(sDB))
	{   	
	   	printc("File count is over threshold\r\n");
	   	return AHC_FALSE;
	}else
	{
		VideoFunc_GetFreeSpace(&ulFreeSpace);
		if( ulFreeSpace >= ulSpaceNeeded )
		{
			printc("Space available\r\n");
			return AHC_TRUE;	
		}else
		{
			printc("File count is enougth, but no freespace\r\n");
			return AHC_FALSE;
		}
		
	}
   
	AHC_UF_SelectDB(sCurDB);
	
	if( ulFreeSpace >= ulSpaceNeeded )
	{
        return AHC_TRUE;
        
    }else{
        return AHC_FALSE;
    }
}

AHC_BOOL AHC_Deletion_RemoveEx(DCF_DB_TYPE sDB, UINT32 uiTime)
{

 	AHC_BOOL	uiResult        = AHC_FALSE;
    UINT64      ulFreeSpace 	= 0;
	UINT64      ulSpaceNeeded 	= 0;
    DCF_DB_TYPE sCurDB;
    UINT32 		ulAudBitRate 	= 128000;//128K
    UINT32      ulVidBitRate	= 0;
    UINT32      i;
	UINT32      ulSpaceNeededInClusters; 

    #define DELETION_DEBUG (1)

    #if (FS_FORMAT_FREE_ENABLE)
    return AHC_TRUE;      //For Format Free, we don't need to check if free space is enough or not
    #endif
    
	AHC_UF_SetFreeChar(0,DCF_SET_ALLOWED,(UINT8*)SEARCH_PHOTO_MOVIE);
	AIHC_GetMovieConfig(AHC_VIDEO_BITRATE, &ulVidBitRate); 

#if (defined(VIDEO_REC_TIMELAPSE_EN) && VIDEO_REC_TIMELAPSE_EN)
    {
        int slVRTimelapse;
        if ((AHC_Menu_SettingGetCB(COMMON_KEY_VR_TIMELAPSE, &slVRTimelapse) == AHC_TRUE) && (slVRTimelapse != PRJ_VR_TIMELAPSE_OFF)){
            UINT32      Framerate, Frate;

            AIHC_GetMovieConfig(AHC_FRAME_RATE, &Frate);
            Framerate = AHC_GetVideoRealFrameRate(Frate) / AHC_VIDRECD_TIME_INCREMENT_BASE;
            VideoFunc_GetTimeLapseBitrate(Framerate, slVRTimelapse, &ulVidBitRate, &ulAudBitRate);
        }
    }
#endif

	AHC_GetVideoRecStorageSpeceNeed(ulVidBitRate, ulAudBitRate, uiTime, &ulSpaceNeeded);
	
    sCurDB = AHC_UF_GetDB();
	AHC_UF_SelectDB(sDB);
#if (DELETION_BY_FILE_NUM)
    //Add check for delete file by num 
    {
        UINT32 uiDeleteNum = 0;
        UINT32 FileCount   = 0;
        AHC_UF_GetTotalFileCount(&FileCount);
        if(FileCount >= AHC_UF_GetFileTH(sDB))
    	{
    		uiDeleteNum  = FileCount - AHC_UF_GetFileTH(sDB) + 1;
		    if(AHC_UF_DeleteFilebyNum(uiDeleteNum) == AHC_TRUE)
            {
		        //uiResult = AHC_TRUE;
		    }
    	}
    }
#endif
    /**
    The original folder has enough space to be deleted.
    */
#if (USE_DIR_BUFFER)
    
    if( sDB == DCF_DB_TYPE_4TH_DB )
    {
        ulSpaceNeededInClusters = STILL_DIR_BUFFER_SIZE /AHC_UF_GetClusterSize(sDB); 
    }
    else
    {
        ulSpaceNeededInClusters = ulSpaceNeeded /AHC_UF_GetClusterSize(sDB);
    }

    ulSpaceNeeded = ulSpaceNeededInClusters * AHC_UF_GetClusterSize(sDB);

    if( AHC_UF_GetFileCluster(sDB)+ ulSpaceNeededInClusters > AHC_UF_GetClusterTH(sDB) )
#else
    if(AHC_UF_GetFileCluster(sDB) > AHC_UF_GetClusterTH(sDB))
#endif
	{
        UINT32 uiSpaceToDelete;

        #if DELETION_DEBUG
        printc("DeletionEx 0x1 \n");
        #endif
        
        //AHC_UF_SelectDB(sDB);

		if( AHC_UF_GetFileCluster(sDB) > AHC_UF_GetClusterTH(sDB) )
        {
            // file size is over threshold
            ulSpaceNeededInClusters += ( AHC_UF_GetFileCluster(sDB) - AHC_UF_GetClusterTH(sDB) );
        }
        else
        {
            ulSpaceNeededInClusters -=  (AHC_UF_GetClusterTH(sDB) - AHC_UF_GetFileCluster(sDB));
        }

        uiSpaceToDelete = ulSpaceNeededInClusters * AHC_UF_GetClusterSize(sDB);

#if 0
        if(sDB == DCF_DB_TYPE_4TH_DB){
            UINT16 uwBytesPerSec;
            UINT16 uwSecPerCluster;
            UINT32 uiTotalCluster;
            UINT32 uiFreelCluster;
            
            AHC_GetStorageMediaGeometry(&uwBytesPerSec, &uwSecPerCluster, &uiTotalCluster, &uiFreelCluster);
            //printc("PIC Cluster0 : %d \n", AHC_UF_GetFileCluster(sDB));
            //printc("PIC Cluster1 : %d \n", AHC_UF_GetClusterTH(sDB));
            //printc("PIC Cluster2 : %d \n", ulStillDirBufferClusterNum);
            
            ulSpaceNeeded = AHC_UF_GetFileCluster(sDB) - AHC_UF_GetClusterTH(sDB) + ulStillDirBufferClusterNum;
            //printc("PIC Cluster : %d \n", ulSpaceNeeded);
            ulSpaceNeeded = ulSpaceNeeded*uwBytesPerSec*uwSecPerCluster;
            //printc("PIC Byte : %d \n", ulSpaceNeeded);
    
        }
#endif

        if(AHC_UF_DeleteCyclicFilebySize(uiSpaceToDelete) == AHC_TRUE){
            uiResult = AHC_TRUE;
            goto Final;
        }
    }

    VideoFunc_GetFreeSpace(&ulFreeSpace);
    
	if( ulFreeSpace >= ulSpaceNeeded )
	{
        #if DELETION_DEBUG
        printc("DeletionEx 0x2 \n");
        #endif
        uiResult = AHC_TRUE;
        goto Final;
        
    }else{
    
        #if DELETION_DEBUG
        printc("DeletionEx 0x3 \n");
        #endif

		#if 1
		i = DCF_DB_TYPE_1ST_DB;
		#else
        for(i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++)
		#endif	
		{
        
            #if DELETION_DEBUG
            printc("DeletionEx 0x4 : %d \n", i);
            #endif
            
            VideoFunc_GetFreeSpace(&ulFreeSpace);
            
            AHC_UF_SelectDB(i);
            
            if( ulFreeSpace >= ulSpaceNeeded ){
                    goto Final;            
            }else{
            
                if(AHC_UF_DeleteCyclicFilebySize(ulSpaceNeeded) == AHC_TRUE){
                    uiResult = AHC_TRUE;
                    goto Final;
                }
            }        
        }
    }
	
Final:
    
    AHC_UF_SelectDB(sCurDB);
	
	return uiResult;
}

AHC_BOOL AHC_Deletion_Romove_Before(AHC_BOOL bFirstTime)
{
	#if (DELETION_STYLE == DELETION_STYLE_C005)
	AHC_Deletion_Romove_By_FileNum_Before(bFirstTime);
	#endif
	return AHC_TRUE;
}
AHC_BOOL AHC_Deletion_Romove(AHC_BOOL bFirstTime)
{
    #if (DELETION_STYLE == DELETION_STYLE_BASIC)
    return AHC_Deletion_Romove_Basic(bFirstTime);
    #elif (DELETION_STYLE == DELETION_STYLE_C005)
    if(MenuSettingConfig()->uiStorageAllocation ==  STORAGE_ALLOCATION_TYPE2)
    return AHC_Deletion_Romove_By_FileNum(bFirstTime);
    else
    return AHC_Deletion_Romove_C005(bFirstTime);
    #elif (DELETION_STYLE == DELETION_CARDV)
    if(DCF_DB_COUNT>=2)
    {
        return AHC_Deletion_RemoveEx(DCF_DB_TYPE_1ST_DB, AHC_GetVRTimeLimit());
    }
    else
    {
        return AHC_Deletion_Remove_CarDV(bFirstTime);
    }
    #endif
}


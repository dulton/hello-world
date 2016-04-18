//==============================================================================
//
//  File        : AHC_Media.c
//  Description : AHC media function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Config_SDK.h"
#include "AHC_Media.h"
#include "AHC_General.h"
#include "AHC_UF.h"
#include "AIHC_DCF.h"
#include "AHC_FS.h"
#include "mmp_lib.h"
#include "mmps_fs.h"
#include "mmpf_fs_api.h"
#include "mmpf_sf.h"
#include "ait_utility.h"
#include "mmpf_pio.h"
#include "AHC_PMU.h"
#include "AHC_OS.h"
#include "AHC_MENU.h" 
#include "Mmpf_storage_api.h"
#include "AHC_Message.h" 
#include "AHC_Callback.h" 

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define MEDIA_ROOT_NAME_LEN  	16
#define SMC_ROOT			 	"SM:\\"
#define MMC_ROOT			 	"SD:\\"
#define MMC1_ROOT			 	"SD1:\\"
/*===========================================================================
 * Global varible
 *===========================================================================*/ 
 
static AHC_MEDIA_ID m_CurMountedMedia                       = AHC_MEDIA_MAX_ID;
static INT8         m_strMediaRootName[MEDIA_ROOT_NAME_LEN] = "SD:\\";  
static UINT32		m_StorageSizeInCluster = 0;
static AHC_BOOL     m_CheckMediaWrite = AHC_TRUE;
UINT8  SDCardUsed	= 0;

/*===========================================================================
 * Extern function
 *===========================================================================*/ 

#if (MENU_MEDIA_EN)
extern AHC_BOOL QuickMediaOperation(UINT8 Op);
#endif

AHC_BOOL	m_EnterNoFitStorageAllocateQuery;

/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
void __LOCAL_API__(){}
#endif

/**
 @brief Get the media storage name

 This function get the media storage name
 
 @retval Media storage name
*/
INT8* AIHC_GetMediaRootName(void)
{    
    return m_strMediaRootName;
}

/**
 @brief Set the DCF root dir name

 This function set the DCF root dir name
 
 @retval AHC_BOOL;
*/
static AHC_BOOL AIHC_SetMediaRootName(INT8 *pRootname)
{
    STRCPY(m_strMediaRootName, pRootname);
    return AHC_TRUE;
}

#if 0
void __AHC_MEDIA_API__(){}
#endif

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
/**
 @brief Get date and time of the captured image.

 This function is used to get the date and time the current image was captured in.
 Parameters:
 @param[out] *puwYear: Return the year information of date.
 @param[out] *puwMonth: Return the month information of date.
 @param[out] *puwDay: Return the day information of date.
 @param[out] *puwHour: Return the hour information of time.
 @param[out] *puwMin: Return the minute information of time.
 @param[out] *puwSec: Return the second information of time.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetImageDateTime(UINT16 *puwYear, UINT8 *puwMonth, UINT8 *puwDay, UINT8 *puwHour, UINT8 *puwMin, UINT8 *puwSec)
{
    UINT32      Index = 0 ;
    AHC_RTC_TIME RtcTime;
    
    if ( AHC_TRUE == AHC_UF_GetCurrentIndex(&Index) ) {
		AHC_UF_GetFileTimebyIndex(Index, &RtcTime);
        
        *puwYear    = RtcTime.uwYear;
        *puwMonth   = RtcTime.uwMonth;
        *puwDay     = RtcTime.uwDay;
        *puwHour    = RtcTime.uwHour;
        *puwMin     = RtcTime.uwMinute;
        *puwSec     = RtcTime.uwSecond;
        
        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }
}

AHC_BOOL AHC_CheckStorageAllocate(void)
{
	UINT32      i;
    #if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
    if(MenuSettingConfig()->uiStorageAllocation ==  STORAGE_ALLOCATION_TYPE2){
        UINT32		FileCount;
        for(i = DCF_DB_TYPE_2ND_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++){
            AHC_UF_SelectDB(i);
			AHC_UF_GetTotalFileCount(&FileCount);
    		#if 0
    		if(i == DCF_DB_TYPE_4TH_DB)  //because system would check count before creating new pic,pic filecount must +1 
            {
                FileCount++;
            }
            #endif
    		if(FileCount > AHC_UF_GetFileTH(i))
    		{
    		    printc("DB[%d] file count > max file count %d!!\r\n",i,AHC_UF_GetFileTH(i));
				return AHC_FALSE;
    		}
    	}
    }
    else
    #endif
    {
	    for(i = DCF_DB_TYPE_1ST_DB; i < DCF_DB_TYPE_MAX_NUMBER; i++){
        
            if(AHC_UF_GetFileCluster(i) > AHC_UF_GetClusterTH(i)) {
			    return AHC_FALSE;
            }
	    }
    }
	return AHC_TRUE;
}	

/**
 @brief Format the specified storage media.

 This function is used to format the specified media.
 Parameters:
 @param[in] byMediaID: The specified storage media.
 @param[out] *pwSuccess: Return the result.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_FormatStorageMedia(UINT8 byMediaID)
{
    MMP_ERR     err = MMP_ERR_NONE;
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;
	
    AHC_UF_UnmountDB();
    
    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_RESOURCE_FAT:
    	#if (STORAGE_FOR_FACTORY_INFO == INFO_NAND_FLASH)
    		bpVolume = "SM:1:\\";
		#elif(STORAGE_FOR_FACTORY_INFO == INFO_SD_CARD)
			bpVolume = "SD:1:\\";        
        #endif
        ulVolNameSize = 6;
    	break;
    case AHC_MEDIA_FACTORY_FAT:
    	#if (STORAGE_FOR_FACTORY_INFO == INFO_NAND_FLASH)
    		bpVolume = "SM:3:\\";
		#elif(STORAGE_FOR_FACTORY_INFO == INFO_SD_CARD)
			bpVolume = "SD:3:\\";
        #elif (STORAGE_FOR_FACTORY_INFO == INFO_SERIAL_FLASH)
        	bpVolume = "SF:1:\\";
        #endif    
        
        ulVolNameSize = 6;
    	break;
    case AHC_MEDIA_USER_FAT:
    	#if (STORAGE_FOR_FACTORY_INFO == INFO_NAND_FLASH)
    		bpVolume = "SM:5:\\";
		#elif(STORAGE_FOR_FACTORY_INFO == INFO_SD_CARD)
			bpVolume = "SD:5:\\";        
        #elif (STORAGE_FOR_FACTORY_INFO == INFO_SERIAL_FLASH)
        	bpVolume = "SF:2:\\";  
        #endif
        
        ulVolNameSize = 6;
    	break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    printc("Format %s\r\n", bpVolume);

    err = MMPS_FS_HighLevelFormat(bpVolume, ulVolNameSize);

    return ((err == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE);
}

#if (FS_FORMAT_FREE_ENABLE)
/**
 @brief Format the specified storage media as FORMAT FREE type.

 This function is used to format the specified media as FORMAT FREE type.
 (Note: Noew only SD card is supported.)
 Parameters:
 @param[in] byMediaID: The specified storage media.
 @param[out] *pwSuccess: Return the result.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_FormatStorageMedia_FormatFree( UINT8 byMediaID )
{
    MMP_ERR err = MMP_ERR_NONE;
    UINT32  ulTickCount = 0;

    AHC_UF_UnmountDB();
    
    AHC_FormatFreeConfigInfo();

    if( AHC_FormatFreeFileNameCB != NULL )
    {
        MMPS_FORMATFREE_RegisterCallback( MMPS_FileSystem_EVENT_FileName_Require, (void*)AHC_FormatFreeFileNameCB );
    }
    
    ulTickCount = OSTimeGet();
    
    err = MMPS_FORMATFREE_Format();
    
    printc("MMPS_FORMATFREE_Format = 0x%x (%d)\r\n", err, ((OSTimeGet() - ulTickCount)*1000)/ OS_TICKS_PER_SEC);

    return ((err == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE);
}

/**
 @brief Check specified storage media if expected FORMAT FREE type.

 This function is used to check specified storage media if expected FORMAT FREE type.
 (Note: Noew only SD card is supported.)
 Parameters:
 @param[in] byMediaID: The specified storage media.
 @param[out] *pwSuccess: Return the result.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_CheckMedia_FormatFree( UINT8 byMediaID )
{
    MMP_ERR err = MMP_ERR_NONE;

    AHC_MountStorageMediaWithoutDCF( byMediaID );

    AHC_FormatFreeConfigInfo();

    if( AHC_FormatFreeFileNameCB != NULL )
    {
        MMPS_FORMATFREE_RegisterCallback( MMPS_FileSystem_EVENT_FileName_Require, (void*)AHC_FormatFreeFileNameCB );
    }
    
    err = MMPS_FORMATFREE_CheckMedia();
    if (err !=MMP_ERR_NONE){
		AHC_DisMountStorageMedia(byMediaID);
	}

    return ((err == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE);
}
#endif

/**
 @brief Mount the specific storage media without DCF.

 This function is used to mount the specific storage media.
 Parameters:
 @param[in] byMediaID: The specified storage media.
 @param[out] *pwSuccess: Return the result.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_MountStorageMediaWithoutDCF(UINT8 byMediaID)
{
    MMP_ERR     err = MMP_ERR_NONE;
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;
    MMP_UBYTE   ubMountCount = 3;
	#if (ENABLE_SD_HOT_PLUGGING) 
    
	#else
    MMP_ULONG64 free_bytes;
	#endif

    m_CheckMediaWrite = AHC_TRUE;
	
    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }

    /**
    @brief	mount the physical device at first.
    */
    
    ubMountCount = 3;
    
    do{

    	if(AHC_MountPhysicalDevice(byMediaID) == AHC_TRUE){
    		break;
    	}else{
    		AHC_UnmountPhysicalDevice(byMediaID);
    	}
    	
    	ubMountCount--;
    	
    }while(ubMountCount > 0);
    
    /**
    @brief	mount the FS device at second.
    */
    ubMountCount = 3;

    do{
	    err = MMPS_FS_MountVolume(bpVolume, ulVolNameSize);
	    if(err != MMP_ERR_NONE){
	    	MMPS_FS_UnmountVolume(bpVolume, ulVolNameSize);
	    }
	    ubMountCount--;
    }while(err != MMP_ERR_NONE && ubMountCount != 0);
    
	#if (!ENABLE_SD_HOT_PLUGGING)

    if(err != MMP_ERR_NONE)
    {
    	free_bytes = 0;
    	return AHC_FALSE;
    }

    if (err == MMP_ERR_NONE) {
        //Get free space only if mounting is successful.
        MMPF_FS_GetVolumeFreeSpace(bpVolume, &free_bytes, MMP_TRUE);
    }
	#else//ENABLE_SD_HOT_PLUGGING
	if (err == MMP_ERR_NONE) 
	{

#if (MENU_MEDIA_EN)	
        QuickMediaOperation(MEDIA_CMD_GET_ATTRIBUTE);
#endif	
		printc("Mount SD %d Without DCF OK.\r\n", byMediaID);
        return AHC_TRUE;
    }
	#endif//ENABLE_SD_HOT_PLUGGING
    else
    {
        return AHC_FALSE;
    }
}

/**
 @brief Mount the specific storage media.

 This function is used to mount the specific storage media.
 Parameters:
 @param[in] byMediaID: The specified storage media.
 @param[out] *pwSuccess: Return the result.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_MountStorageMedia(UINT8 byMediaID)
{
    MMP_ERR     err = MMP_ERR_NONE;
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;
    MMP_UBYTE   ubMountCount = 3;
    MMP_ULONG64 free_bytes;
	#if (ENABLE_SD_HOT_PLUGGING) 
    AHC_BOOL 	mount_status = AHC_TRUE;
	#endif
	
    m_CheckMediaWrite = AHC_TRUE;
	
    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_FACTORY_FAT:
    	bpVolume = "SF:1:\\";
        ulVolNameSize = 5;
    	break;
    case AHC_MEDIA_USER_FAT:
    	bpVolume = "SF:2:\\";
        ulVolNameSize = 5;
    	break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    MEMSET(m_strMediaRootName,0, sizeof(m_strMediaRootName));
    STRCPY(m_strMediaRootName, bpVolume);

    /**
    @brief	mount the physical device at first.
    */
    
    ubMountCount = 3;
    
    do{

    	if(AHC_MountPhysicalDevice(byMediaID) == AHC_TRUE){
    		break;
    	}else{
    		AHC_UnmountPhysicalDevice(byMediaID);
    	}
    	
    	ubMountCount--;
    	
    }while(ubMountCount > 0);
    
    /**
    @brief	mount the FS device at second.
    */
    ubMountCount = 3;

    do{
	    err = MMPS_FS_MountVolume(bpVolume, ulVolNameSize);
	    if(err != MMP_ERR_NONE){
	    	MMPS_FS_UnmountVolume(bpVolume, ulVolNameSize);
	    }
	    ubMountCount--;
    }while(err != MMP_ERR_NONE && ubMountCount != 0);
    
	#if (!ENABLE_SD_HOT_PLUGGING)

    if(err != MMP_ERR_NONE)
    {
    	free_bytes = 0;
    }

    if (err == MMP_ERR_NONE) {
        //Get free space only if mounting is successful.
        MMPF_FS_GetVolumeFreeSpace(bpVolume, &free_bytes, MMP_TRUE);
        m_CurMountedMedia = byMediaID;
        
        AHC_LoadSystemFile();
        
        AHC_EnableDir();
        
        err = AHC_UF_MountDB( MAX_DCF_MEM );

        if((AHC_MEDIA_MMC == byMediaID)|| (AHC_MEDIA_MMC1 == byMediaID)){

            //AHC_SDMMC_SetMountState(SDMMC_1STIN_2NDOUT);

            #if (VR_SEAMLESS)
            printc("Clear SD space \r\n");
			#endif
        }

		m_CheckMediaWrite = err;
        return err;
    }
	#else
	if (err == MMP_ERR_NONE) {
       
        m_CurMountedMedia = byMediaID;

        MMPF_FS_GetVolumeFreeSpace(bpVolume, &free_bytes, MMP_TRUE);
#if (MENU_MEDIA_EN)	
        QuickMediaOperation(MEDIA_CMD_GET_ATTRIBUTE);
#endif
		
        AHC_EnableDir();
        //DCF DB mount
        mount_status = AHC_UF_MountDB( MAX_DCF_MEM );

        if(AHC_TRUE == mount_status){

            if((AHC_MEDIA_MMC == byMediaID)|| (AHC_MEDIA_MMC1 == byMediaID)){
            
                //AHC_SDMMC_SetMountState(SDMMC_1STIN_2NDOUT);

                #if (VR_SEAMLESS) && !defined(CAR_DV)
                printc("Clear SD space \r\n");

                /*
                check the storage allocation of DTDCF.
                */
                if(AHC_MEDIA_MMC == byMediaID){
					if(AHC_CheckStorageAllocate() == AHC_FALSE) {

						#if(defined(DEVICE_GPIO_2NDSD_PLUG))
						if(	((AHC_MEDIA_MMC == byMediaID) && AHC_IsSDInserted()))
						#else//DEVICE_GPIO_2NDSD_PLUG
						if((AHC_MEDIA_MMC == byMediaID) && AHC_IsSDInserted())
						#endif//DEVICE_GPIO_2NDSD_PLUG 
						{
							if(!AHC_Check2SysCalibMode2()) {

								m_EnterNoFitStorageAllocateQuery = AHC_TRUE;
								AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_NO_FIT_STORAGE_ALLOCATE, 0);
	
								return AHC_FALSE;
							}
						}
					}	
				}

                #if(defined(DEVICE_GPIO_2NDSD_PLUG))
				if(	((AHC_MEDIA_MMC == byMediaID) && AHC_IsSDInserted()) ) //	||
					//((AHC_MEDIA_MMC1 == byMediaID) && AHC_Is2ndSDInserted())) // Do not check SD2.
				#else//DEVICE_GPIO_2NDSD_PLUG
				if((AHC_MEDIA_MMC == byMediaID) && AHC_IsSDInserted())
				#endif//DEVICE_GPIO_2NDSD_PLUG 
				{
				    DCF_DB_TYPE sCurDB = DCF_DB_TYPE_1ST_DB;

                	sCurDB = AHC_UF_GetDB(); 
                	
                	AHC_UF_SelectDB(DCF_DB_TYPE_1ST_DB);
                	AHC_Deletion_Romove(AHC_TRUE);
                	AHC_UF_SelectDB(sCurDB);
                }
                #endif
            }

		    printc("Mount SD%d OK.\r\n", byMediaID);
			m_CheckMediaWrite = AHC_TRUE;
            return AHC_TRUE;
        }
        else{
            m_CurMountedMedia = AHC_MEDIA_MAX_ID;
            if((AHC_MEDIA_MMC == byMediaID)|| (AHC_MEDIA_MMC1 == byMediaID)){
                //AHC_SDMMC_SetMountState(AHC_FALSE);              
            }

            AHC_DisMountStorageMedia(byMediaID);
            printc("Mount SD%d FAIL.\r\n", byMediaID);
			m_CheckMediaWrite = AHC_FALSE;
            return AHC_FALSE;
        }
    }
	#endif
    else{
        return AHC_FALSE;
    }
}

AHC_BOOL AHC_GetMountStorageMediaStatus(void) {
	return m_CheckMediaWrite;
}	

/**
 @brief Unmount the specific storage media.

 This function is used to ummount the specific storage media.
 Parameters:
 @param[in] byMediaID: The mounted storage media.
 @retval AHC_TRUE Success.
*/

AHC_BOOL AHC_DisMountStorageMediaEx(UINT8 byMediaID, AHC_BOOL bUnmountPhysical )
{
    MMP_ERR     err = MMP_ERR_NONE;
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;

    #if 0 //Mark and Rogers Check!!
    if (byMediaID != m_CurMountedMedia) {
        return AHC_FALSE;
    }
    #endif
	
	switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_FACTORY_FAT:
     	bpVolume = "SF:1:\\";
        ulVolNameSize = 5;
    	break;
    case AHC_MEDIA_USER_FAT:
     	bpVolume = "SF:2:\\";
        ulVolNameSize = 5;
    	break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    #if (ENABLE_SD_HOT_PLUGGING)
    if((AHC_MEDIA_MMC == byMediaID)|| (AHC_MEDIA_MMC1 == byMediaID))
    {
        //AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);
    }
	#endif

    if (byMediaID == m_CurMountedMedia) {
    	AHC_UnloadSystemFile();
    	
    	AHC_UF_UnmountDB();
    	
    	printc("Unmount SD%d DCF\r\n", byMediaID);
    }
	
	#if(defined(DEVICE_GPIO_2NDSD_PLUG))
	//NOP
	#else
    if( !bUnmountPhysical )
    {
        return AHC_TRUE;
    }
    #endif//DEVICE_GPIO_2NDSD_PLUG
    
    err = MMPS_FS_UnmountVolume(bpVolume, ulVolNameSize);
    
    if (err == MMP_ERR_NONE) {
    	AHC_UnmountPhysicalDevice(byMediaID);

        if (m_CurMountedMedia == byMediaID)
            m_CurMountedMedia = AHC_MEDIA_MAX_ID; //reset
    
    	printc("Unmount SD%d OK\r\n", byMediaID);
    	
    	if(AHC_SDMMC_GetMountState() == AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT && byMediaID == AHC_MEDIA_MMC1)
    	{
    		AHC_DisMountStorageMediaOnlyPhysical(AHC_MEDIA_MMC);
    	}
    	else if(AHC_SDMMC_GetMountState() == AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT && byMediaID == AHC_MEDIA_MMC)
    	{
    		AHC_DisMountStorageMediaOnlyPhysical(AHC_MEDIA_MMC1);
    	}
    	
    	AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);	

#if(LIMIT_MAX_LOCK_FILE_NUM==1)
        {
            extern UINT32	m_ulLockFileNum;
        	m_ulLockFileNum = 0;
            printc(FG_YELLOW("%s:%d: reset m_ulLockFileNum to 0\r\n"), __func__, __LINE__);
        }
#endif
    
        return AHC_TRUE;
    }
    else {
    	printc("Unmount SD%d Failed\r\n", byMediaID);
    	
    	if(AHC_SDMMC_GetMountState() == AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT && byMediaID == AHC_MEDIA_MMC1)
    	{
    		AHC_DisMountStorageMediaOnlyPhysical(AHC_MEDIA_MMC);
    	}
    	else if(AHC_SDMMC_GetMountState() == AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT && byMediaID == AHC_MEDIA_MMC)
    	{
    		AHC_DisMountStorageMediaOnlyPhysical(AHC_MEDIA_MMC1);
    	}
    	
    	AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);	
        return AHC_FALSE;
    }
}

AHC_BOOL AHC_DisMountStorageMediaOnlyPhysical(UINT8 byMediaID)
{
    MMP_ERR     err = MMP_ERR_NONE;
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;

    #if 0 //Mark and Rogers Check!!
    if (byMediaID != m_CurMountedMedia) {
        return AHC_FALSE;
    }
    #endif
	
	switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
	
    err = MMPS_FS_UnmountVolume(bpVolume, ulVolNameSize);
    
    if (err == MMP_ERR_NONE) {
    	AHC_UnmountPhysicalDevice(byMediaID);

        if (m_CurMountedMedia == byMediaID)
            m_CurMountedMedia = AHC_MEDIA_MAX_ID; //reset
    
    	printc("Unmount Physical SD%d OK\r\n", byMediaID);
    	
    	switch(byMediaID) 
    	{
    		case AHC_MEDIA_MMC:
        	{
        		if(AHC_SDMMC_GetMountState() == AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT)
        		{
        			AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT);	
        		}
        	}	
        	break;
    		case AHC_MEDIA_MMC1:
			{
        		if(AHC_SDMMC_GetMountState() == AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT)
        		{
        			AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT);	
        		}
        	}
        	break;
    	}
    	
        return AHC_TRUE;
    }
    else {
    	printc("Unmount Physical SD%d Failed\r\n", byMediaID);
    	
        return AHC_FALSE;
    }
}

/**
 @brief Unmount the specific storage media.

 This function is used to ummount the specific storage media.
 Parameters:
 @param[in] byMediaID: The mounted storage media.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_DisMountStorageMedia(UINT8 byMediaID)
{
    return AHC_DisMountStorageMediaEx(byMediaID, AHC_TRUE);
}

AHC_BOOL AHC_GetStorageMediaStatus()
{
#if (CHIP == P_V2)
    AHC_BOOL bReturn = 0;
    UINT8	 buff[512];
    UINT8	 bValue = 0;

    AHC_GetGPIO(AHC_PIO_REG_LGPIO28,&bValue);
    if(bValue == AHC_FALSE)
    {
    	if(uiGetCurrentState() == UI_VIDEO_STATE)
    	{
    	    extern MMP_USHORT MMPF_MP4VENC_GetStatus(void);
    		if(MMPF_MP4VENC_GetStatus()!= 0x01)
    		bReturn = MMPF_STORAGE_DevRead(1,0, 0x01,1, buff );
    	}else
    	{
    		bReturn = MMPF_STORAGE_DevRead(1,0, 0x01,1, buff );
    	}
    	
    	if(bReturn == 0)
	    return AHC_TRUE;
    	else
    	return AHC_FALSE;
    }else
    {
    	return AHC_TRUE;
    }
#endif   

#if (CHIP == MCR_V2) 
    return AHC_TRUE; //Rogers:***  //TODO
#endif    
    
}

/**
 @brief Get free space of the specific storage media.

 This function is used to get free space of the specific storage media.
 Parameters:
 @param[in] byMediaID: The specified storage media.
 @param[out] *pwSuccess: Return the result.
 @param[out] *pullFreeSize: Return the free space.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetStorageOtherMediaStatus(UINT8 byMediaID, UINT64 *pullFreeSize)
{
    MMP_ERR     err = MMP_ERR_NONE;
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;

    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    err = MMPS_FS_IsVolumeMounted(bpVolume, ulVolNameSize);

    if (err == MMP_ERR_NONE) {
        err = MMPS_FS_GetFreeSpace(bpVolume, ulVolNameSize, pullFreeSize);
    }

    if (err == MMP_ERR_NONE) {
        return AHC_TRUE;
    }
    else {
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

/**
 @brief Get geometry of the mounted storage media.

 This function is used to get BPB information of the mounted storage media.
 Parameters:
 @param[out] *pwSuccess: Return the result.
 @param[out] *pwBytesPerSec: Return number of bytes per sector.
 @param[out] *pwSecPerCluster: Return number of sectors per cluster.
 @param[out] *pwTotalCluster: Return total number of cluster.
 @param[out] *pwFreelCluster: Return the number of free cluster.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetStorageMediaGeometry(UINT16 *pwBytesPerSec, UINT16 *pwSecPerCluster, UINT32 *pwTotalCluster, UINT32 *pwFreelCluster)
{
    MMP_BYTE    		*bpVolume="";
    MMP_ULONG   		ulVolNameSize;
    MMPS_FS_DISK_INFO 	volinfo;

    *pwBytesPerSec 		= 0;
    *pwSecPerCluster 	= 0;
    *pwTotalCluster 	= 0;
    *pwFreelCluster 	= 0;

    switch(m_CurMountedMedia) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    if (MMPS_FS_GetVolumeInfo(bpVolume, ulVolNameSize, &volinfo) == MMP_ERR_NONE) {
        *pwBytesPerSec 		= volinfo.usBytesPerSector;
        *pwSecPerCluster 	= volinfo.usSecPerCluster;
        *pwTotalCluster 	= volinfo.ulTotalClusters;
        *pwFreelCluster 	= volinfo.ulFreeClusters;
    }
    else {
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

AHC_BOOL AHC_GetStorageMediaInfo(AHC_FS_DISK_INFO* sMediaInfo)
{
    MMP_BYTE    		*bpVolume="";
    MMP_ULONG   		ulVolNameSize;

    switch(m_CurMountedMedia) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    if (AHC_FS_GetVolumeInfo(bpVolume, ulVolNameSize, sMediaInfo) != MMP_ERR_NONE) {
        return AHC_FALSE;
    }

    return AHC_TRUE;
}

AHC_BOOL AHC_SetMediaAttr(UINT8 byMediaID, AHC_MEDIA_ATTR bMediaAttr, UINT32 uiParam)
{
    MMP_BYTE    *bpVolume="";
    MMP_ULONG   ulVolNameSize;

    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }

    switch(bMediaAttr) {
    case AHC_MEDIA_ATTR_WP:
    case AHC_MEDIA_ATTR_VOL_ID:
    case AHC_MEDIA_ATTR_TYPE:
    case AHC_MEDIA_ATTR_STATUS:
    default:
        //Todo
        //Implementation
        break;
    }
    return AHC_TRUE;
}

AHC_BOOL AHC_GetMediaAttr(UINT8 byMediaID, AHC_MEDIA_ATTR bMediaAttr, UINT32 *puiParam)
{
    MMP_BYTE    *bpVolume;
    MMP_ULONG   ulVolNameSize;

    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        ulVolNameSize = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        ulVolNameSize = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }

    switch(bMediaAttr) {
    case AHC_MEDIA_ATTR_WP:
    case AHC_MEDIA_ATTR_VOL_ID:
    case AHC_MEDIA_ATTR_TYPE:
    case AHC_MEDIA_ATTR_STATUS:
    default:
        //Todo
        //Implementation
        break;
    }
    return AHC_TRUE;
}

AHC_BOOL AHC_SetMediaPath(AHC_MEDIA_ID path)
{
	m_CurMountedMedia = path;
	return AHC_TRUE;
}

AHC_MEDIA_ID AHC_GetMediaPath()
{ 
	return m_CurMountedMedia;
}

AHC_BOOL AHC_GetVolumeName(INT8* pchVolume, UINT32* puiVolNameSize)
{
    switch(m_CurMountedMedia) {
    case AHC_MEDIA_MMC:
        //pchVolume = "SD:\\";
        *puiVolNameSize = 4;
        MEMCPY(pchVolume, "SD:\\", *puiVolNameSize);
        break;
    case AHC_MEDIA_MMC1:
        //pchVolume = "SD1:\\";
        *puiVolNameSize = 5;
        MEMCPY(pchVolume, "SD1:\\", *puiVolNameSize);
        break;
    case AHC_MEDIA_SMC:
        //pchVolume = "SM:\\";
        *puiVolNameSize = 4;
        MEMCPY(pchVolume, "SM:\\", *puiVolNameSize);
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    return AHC_TRUE;
}

AHC_BOOL AHC_SwitchMediaPath(void) //SD->NAND, NAND->SD
{
	AHC_MEDIA_ID currentPath;
	AHC_BOOL	 status;

	currentPath = AHC_GetMediaPath();

	if(currentPath == AHC_MEDIA_MMC)
	{
		AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
		AHC_SetMediaPath(AHC_MEDIA_SMC);
		AIHC_SetMediaRootName(SMC_ROOT);
		status = AHC_MountStorageMedia(AHC_MEDIA_SMC);	
	}
	else
	{
		AHC_DisMountStorageMedia(AHC_MEDIA_SMC);
		AHC_SetMediaPath(AHC_MEDIA_MMC);
		AIHC_SetMediaRootName(MMC_ROOT);
		status = AHC_MountStorageMedia(AHC_MEDIA_MMC);

		if(!status)
		{
			AHC_SetMediaPath(AHC_MEDIA_SMC);
			AIHC_SetMediaRootName(SMC_ROOT);
			status = AHC_MountStorageMedia(AHC_MEDIA_SMC);	
		}
	}
	return status;
}

AHC_BOOL AHC_RemountDevices(MEDIASETING_SETTING media)
{
	AHC_BOOL	 status;
	UINT8 oriMedia;
    
	switch(media)
	{
	
#if USING_SF_IF
		case MEDIA_SETTING_USERFAT:
			 AHC_DisMountStorageMedia(AHC_MEDIA_USER_FAT);
			 status = AHC_MountStorageMedia(AHC_MEDIA_USER_FAT);
		break;
		case MEDIA_SETTING_FACFAT:
			 AHC_DisMountStorageMedia(AHC_MEDIA_FACTORY_FAT);
			 status = AHC_MountStorageMedia(AHC_MEDIA_FACTORY_FAT);	
		break;
#endif

#if USING_SD_IF
		case MEDIA_SETTING_SD_CARD:
			 printc("remount MMC\r\n");
			 #if USING_SM_IF 
			 AHC_DisMountStorageMedia(AHC_MEDIA_SMC);
			 #endif
			 // Why we need to remount here ? //CZ patch...20160204
			 if (MMPS_FS_IsVolumeMounted("SD:\\",4) != 0)
			 	AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
             #if USING_SD1_IF
             if (MMPS_FS_IsVolumeMounted("SD1:\\",5) != 0)
			 	AHC_DisMountStorageMedia(AHC_MEDIA_MMC1);
             #endif
             
             oriMedia = MenuSettingConfig()->uiMediaSelect;
             MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD; // Set to 1st SD card. Because user choose to remount DCF to 1st SD  card.
             															 // Need verify. by Matt.

			 AHC_SetMediaPath(AHC_MEDIA_MMC);
			 AIHC_SetMediaRootName(MMC_ROOT);
			 status = AHC_MountStorageMedia(AHC_MEDIA_MMC);
			 
			 if(status == AHC_TRUE || m_EnterNoFitStorageAllocateQuery == AHC_TRUE)
			 {
			 	//MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD;
			 
			 	if(AHC_Is2ndSDInserted())
			 	{
			 		status = AHC_MountStorageMediaWithoutDCF(AHC_MEDIA_MMC1);
			 		
			 		if(status == AHC_TRUE)
			 		{
			 			AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT);
			 		}
			 		else
			 		{
			 			AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT);
			 		}
			 	}
			 	else
			 	{
			 		AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT);
			 	}
			 }
			 else
			 {
			 	MenuSettingConfig()->uiMediaSelect = oriMedia; // Restore to original setting due to 1st SD card DCF mount failed!!
			 	
			 	#if(defined(DEVICE_GPIO_2NDSD_PLUG))
			 	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
			 	if(AHC_Is2ndSDInserted())
			 	{
			 		AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
			 		status = AHC_MountStorageMedia(AHC_MEDIA_MMC1);
			 		
			 		if(status == AHC_TRUE)
			 		{
			 			MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD1;
			 			AHC_SetMediaPath(AHC_MEDIA_MMC1);
			 			AIHC_SetMediaRootName(MMC1_ROOT);
			 			
			 			if(AHC_IsSDInserted())
			 			{
			 				status = AHC_MountStorageMediaWithoutDCF(AHC_MEDIA_MMC1);
			 				if(status == AHC_TRUE)
			 				{
			 					AHC_SDMMC_SetMountState(AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT);
			 				}
			 				else
			 				{
			 					AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT);
			 				}
			 			}
			 			else
			 			{
			 				AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT);
			 			}
			 		}
			 		else
			 		{
			 			AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);
			 		}
			 	}
			 	else
			 	#endif//TWOSD_WORK_MODEL
			 	#endif//DEVICE_GPIO_2NDSD_PLUG
			 	{
			 		AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);
			 	}
			 }
		break;
#endif
#if USING_SD1_IF
		case MEDIA_SETTING_SD_CARD1:
			 printc("remount MMC1\r\n");
			 #if USING_SM_IF 
			 AHC_DisMountStorageMedia(AHC_MEDIA_SMC);
			 #endif
             #if USING_SD_IF
			 AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
             #endif
			 AHC_DisMountStorageMedia(AHC_MEDIA_MMC1);

             oriMedia = MenuSettingConfig()->uiMediaSelect;
             MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD1; // Set to 2nd SD card. Because user choose to remount DCF to 2nd SD  card.
			 
			 AHC_SetMediaPath(AHC_MEDIA_MMC1);
			 AIHC_SetMediaRootName(MMC1_ROOT);
			 status = AHC_MountStorageMedia(AHC_MEDIA_MMC1);
			 
			 if(status == AHC_TRUE)
			 {
			 	//MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD1;
			 
			 	if(AHC_IsSDInserted())
			 	{
			 		status = AHC_MountStorageMediaWithoutDCF(AHC_MEDIA_MMC);
			 		
			 		if(status == AHC_TRUE)
			 		{
			 			AHC_SDMMC_SetMountState(AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT);
			 		}
			 		else
			 		{
			 			AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT);
			 		}
			 	}
			 	else
			 	{
			 		AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT);
			 	}
			 }
			 else
			 {
			 	MenuSettingConfig()->uiMediaSelect = oriMedia;
			 	
			 	#if(defined(DEVICE_GPIO_2NDSD_PLUG))
			 	#if(TWOSD_WORK_MODEL == TWOSD_WORK_MODEL_COMPLEMENTARY)
			 	if(AHC_IsSDInserted())
			 	{
			 		AHC_DisMountStorageMedia(AHC_MEDIA_MMC1);
			 		status = AHC_MountStorageMedia(AHC_MEDIA_MMC);
			 		
			 		if(status == AHC_TRUE)
			 		{
			 			MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD;
			 			
			 			if(AHC_Is2ndSDInserted())
			 			{
			 				status = AHC_MountStorageMediaWithoutDCF(AHC_MEDIA_MMC1);
			 				AHC_SetMediaPath(AHC_MEDIA_MMC);
			 				AIHC_SetMediaRootName(MMC_ROOT);
			 				
			 				if(status == AHC_TRUE)
			 				{
			 					AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT);
			 				}
			 				else
			 				{
			 					AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT);
			 				}
			 			}
			 			else
			 			{
			 				AHC_SDMMC_SetMountState(AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT);
			 			}
			 		}
			 		else
			 		{
			 			AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);
			 		}
			 	}
			 	else
			 	#endif//TWOSD_WORK_MODEL
			 	#endif
			 	{
			 		AHC_SDMMC_SetMountState(AHC_SD1_NO_MOUNT_SD2_NO_MOUNT);
			 	}
			 }
		break;
#endif
#if USING_SM_IF
		case MEDIA_SETTING_INTERNAL:
			printc("remount SMC\r\n");
            #if USING_SD_IF
			AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
            #endif
            #if USING_SD1_IF
			AHC_DisMountStorageMedia(AHC_MEDIA_MMC1);
            #endif
			 AHC_DisMountStorageMedia(AHC_MEDIA_SMC);
			 AHC_SetMediaPath(AHC_MEDIA_SMC);
			 AIHC_SetMediaRootName(SMC_ROOT);
			 status = AHC_MountStorageMedia(AHC_MEDIA_SMC);
		break;
#endif
		default:
            #if USING_SM_IF
            AHC_DisMountStorageMedia(AHC_MEDIA_SMC);
            #endif
            #if USING_SD_IF
            AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
            #endif
            #if USING_SD1_IF
            AHC_DisMountStorageMedia(AHC_MEDIA_MMC1);
            #endif

#if USING_SD_IF
			AHC_SetMediaPath(AHC_MEDIA_MMC);
			AIHC_SetMediaRootName(MMC_ROOT);
			status = AHC_MountStorageMedia(AHC_MEDIA_MMC);
#elif USING_SD1_IF
			AHC_SetMediaPath(AHC_MEDIA_MMC1);
			AIHC_SetMediaRootName(MMC1_ROOT);
			status = AHC_MountStorageMedia(AHC_MEDIA_MMC1);
#elif USING_SM_IF
			AHC_SetMediaPath(AHC_MEDIA_SMC);
			AIHC_SetMediaRootName(SMC_ROOT);
			status = AHC_MountStorageMedia(AHC_MEDIA_SMC);
#endif
		break;
	}
	
	printc("Media Path: %d\r\n", AHC_GetMediaPath());
	printc("Root Name: %s\r\n", AIHC_GetMediaRootName());
	printc("Mount Status: %d\r\n", AHC_SDMMC_GetMountState());
	printc("SDMMC Status: %d\r\n", AHC_SDMMC_GetState());
	printc("MediaSelect = %d\r\n", MenuSettingConfig()->uiMediaSelect);
	
	return status;
}

AHC_BOOL AHC_IsSDInserted(void)
{
#if (SD_DETECT_EN && defined(DEVICE_GPIO_SD_PLUG))
    MMP_UBYTE tempValue;

    #if (!defined(DEVICE_GPIO_2NDSD_PLUG))
    if(DEVICE_GPIO_SD_PLUG == MMP_GPIO_MAX)
		return AHC_FALSE;
	#endif
    MMPF_PIO_EnableOutputMode(DEVICE_GPIO_SD_PLUG, MMP_FALSE, MMP_TRUE);
    MMPF_PIO_GetData(DEVICE_GPIO_SD_PLUG, &tempValue);
    #ifdef DEVICE_GPIO_SD_PLUG_LEVEL
	    #if (DEVICE_GPIO_SD_PLUG_LEVEL == GPIO_HIGH)
		if(tempValue)
		{
			SDCardUsed = 0;
			return AHC_TRUE;
		}
		else
		{
			#if (SD_DETECT_EN && defined(DEVICE_GPIO_2NDSD_PLUG))
		   
		    if(DEVICE_GPIO_2NDSD_PLUG == MMP_GPIO_MAX)
				return AHC_FALSE;
				
			MMPF_PIO_EnableOutputMode(DEVICE_GPIO_2NDSD_PLUG, MMP_FALSE, MMP_TRUE);
		    MMPF_PIO_GetData(DEVICE_GPIO_2NDSD_PLUG, &tempValue);
		    
		    if(tempValue)
			{
				SDCardUsed = 1;
				return AHC_FALSE;
			}
			#endif
		}
		#else
		if(!tempValue)
		{
			SDCardUsed = 0;
			return AHC_TRUE;
		}
		else
		{
			#if (SD_DETECT_EN && defined(DEVICE_GPIO_2NDSD_PLUG))
		   
		    if(DEVICE_GPIO_2NDSD_PLUG == MMP_GPIO_MAX)
				return AHC_FALSE;
				
			MMPF_PIO_EnableOutputMode(DEVICE_GPIO_2NDSD_PLUG, MMP_FALSE, MMP_TRUE);
		    MMPF_PIO_GetData(DEVICE_GPIO_2NDSD_PLUG, &tempValue);
		    
		    if(!tempValue)
			{
				SDCardUsed = 1;
				return AHC_FALSE;
			}
			#endif
		}
		#endif
	#else
		if(!tempValue)
		{
			SDCardUsed = 0;
			return AHC_TRUE;
		}
		else
		{
			#if (SD_DETECT_EN && defined(DEVICE_GPIO_2NDSD_PLUG))
		   
		    if(DEVICE_GPIO_2NDSD_PLUG == MMP_GPIO_MAX)
				return AHC_FALSE;
				
			MMPF_PIO_EnableOutputMode(DEVICE_GPIO_2NDSD_PLUG, MMP_FALSE, MMP_TRUE);
		    MMPF_PIO_GetData(DEVICE_GPIO_2NDSD_PLUG, &tempValue);
		    
		    if(!tempValue)
			{
				SDCardUsed = 1;
				return AHC_FALSE;
			}
			#endif
		}
	#endif
	
	return AHC_FALSE;

#else
    return AHC_TRUE;
	
#endif
}

AHC_BOOL AHC_Is2ndSDInserted(void)
{
#if (SD_DETECT_EN && defined(DEVICE_GPIO_2NDSD_PLUG))
    MMP_UBYTE tempValue;

    if(DEVICE_GPIO_2NDSD_PLUG == MMP_GPIO_MAX)
		return AHC_FALSE;
		
	MMPF_PIO_EnableOutputMode(DEVICE_GPIO_2NDSD_PLUG, MMP_FALSE, MMP_TRUE);
    MMPF_PIO_GetData(DEVICE_GPIO_2NDSD_PLUG, &tempValue);
    
    if(AHC_IsSDInserted())
    {
    	SDCardUsed = 0;
    }else
    {
    	if(!tempValue)
		SDCardUsed = 1;
    }
    
    
	return (tempValue)?(AHC_FALSE):(AHC_TRUE);
	
#else
    return AHC_FALSE;
	
#endif
}

AHC_BOOL AHC_IsSDWriteProtect(void)
{
#if (SD_WRITE_PROTECT_EN && defined(SD_WRITE_PROTECT_GPIO))
    MMP_UBYTE tempValue;

    if(SD_WRITE_PROTECT_GPIO == MMP_GPIO_MAX)
    return AHC_FALSE;

    MMPF_PIO_EnableOutputMode(SD_WRITE_PROTECT_GPIO, MMP_FALSE, MMP_TRUE);
    MMPF_PIO_GetData(SD_WRITE_PROTECT_GPIO, &tempValue);

	return (tempValue)?(AHC_TRUE):(AHC_FALSE); 
#else
	return	AHC_FALSE;
#endif
}

/**
@brief the period of clearing the cache. unit: byte.

	ex: 8Mbps video. the max. loss is 300ms.

	period size = 300*8/8000 = 0.3MB = 300KB

	MMPF_FS_SetClearCachePeriod("SD:\\", 300*1024)
*/
AHC_BOOL AHC_SetClearCachePeriod(UINT8 byMediaID, UINT32 ulSize)
{
    MMP_BYTE    *bpVolume="";

    switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        break;
    case AHC_MEDIA_MEMDISK:
    default:
	
        return AHC_FALSE;
    }

	//MMPF_FS_SetClearCachePeriod(bpVolume, ulSize); //Rogers:*** bpVolume --> ulFileID, 是否改成file open後設定??
	
	return AHC_TRUE;
}

AHC_BOOL AHC_MountPhysicalDevice(UINT8 byMediaID)
{
	MMP_BYTE    		*pbyVolume="";
	UINT32				uiVolLength = 0;
	AHC_FS_IOCTL_PARA 	sPara;
	UINT32				uiBuffer;
	
	switch(byMediaID) {
	
    case AHC_MEDIA_MMC:
        pbyVolume = "SD:\\";
        uiVolLength = 4;
        break;
    case AHC_MEDIA_MMC1:
        pbyVolume = "SD1:\\";
        uiVolLength = 5;
        break;
    case AHC_MEDIA_SMC:
        pbyVolume = "SM:\\";
        uiVolLength = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }

    if(AHC_FS_IOCtl(pbyVolume, uiVolLength, AHC_FS_CMD_MOUNT_MEDIUM, &sPara, &uiBuffer) == AHC_ERR_NONE){
    	printc("Mount P Success \r\n");
    	return AHC_TRUE;
    }else{
    	printc("Mount P Fail \r\n");
    	return AHC_FALSE;
    }
}

AHC_BOOL AHC_UnmountPhysicalDevice(UINT8 byMediaID)
{
	MMP_BYTE    		*pbyVolume="";
	UINT32				uiVolLength = 0;
	AHC_FS_IOCTL_PARA 	sPara;
	UINT32				uiBuffer;

	switch(byMediaID) {
	
    case AHC_MEDIA_MMC:
        pbyVolume = "SD:\\";
        uiVolLength = 4;
        break;
    case AHC_MEDIA_MMC1:
        pbyVolume = "SD1:\\";
        uiVolLength = 5;
        break;
    case AHC_MEDIA_SMC:
        pbyVolume = "SM:\\";
        uiVolLength = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    if(AHC_FS_IOCtl(pbyVolume, uiVolLength, AHC_FS_CMD_UNMOUNT_MEDIUM, &sPara, &uiBuffer) == AHC_ERR_NONE){
    	printc("Unmount P Success \r\n");
    	return AHC_TRUE;
    }else{
    	printc("Unmount P Fail \r\n");
    	return AHC_FALSE;
    }
}

AHC_BOOL AHC_UnmountVolume(UINT8 byMediaID)
{
	MMP_BYTE    		*pbyVolume;
	UINT32				uiVolLength = 0;
	
	switch(byMediaID) {
	
    case AHC_MEDIA_MMC:
        pbyVolume = "SD:\\";
        uiVolLength = 4;
        break;
    case AHC_MEDIA_MMC1:
        pbyVolume = "SD1:\\";
        uiVolLength = 5;
        break;
    case AHC_MEDIA_SMC:
        pbyVolume = "SM:\\";
        uiVolLength = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    if(AHC_FS_IsVolumeMounted(pbyVolume, uiVolLength) == AHC_ERR_NONE)
    {
    	printc("Unmount %s \r\n", pbyVolume);        
        AHC_FS_UnmountVolume(pbyVolume, uiVolLength);

    	return AHC_TRUE;
    }
    
    return AHC_TRUE;
}


/*
	@brief Reset SD device
*/
AHC_BOOL AHC_ResetPhysicalDevice(UINT8 byMediaID)
{
	//extern RTNA_LCD_InitMainLCD(void);

	MMP_BYTE    		*bpVolume="";
	MMP_BYTE			iVolLength = 0;	
	AHC_FS_IOCTL_PARA 	sPara;
	MMP_BYTE			buffer[256];
	
	printc("Turn off LDO 1\r\n");
	AHC_PMUCtrl_LDO(AHC_PMU_LDO_ID_1, AHC_FALSE);
	AHC_OS_Sleep(10);
	printc("Turn on LDO 1\r\n");
	AHC_PMUCtrl_LDO(AHC_PMU_LDO_ID_1, AHC_TRUE);
	AHC_OS_Sleep(10);
	
	switch(byMediaID) {
    case AHC_MEDIA_MMC:
        bpVolume = "SD:\\";
        iVolLength = 4;
        break;
    case AHC_MEDIA_MMC1:
        bpVolume = "SD1:\\";
        iVolLength = 5;
        break;
    case AHC_MEDIA_SMC:
        bpVolume = "SM:\\";
        iVolLength = 4;
        break;
    case AHC_MEDIA_MEMDISK:
    default:
        return AHC_FALSE;
    }
    
    AHC_FS_IOCtl(bpVolume, iVolLength, AHC_FS_CMD_RESET_STORAGE, &sPara, buffer);
    
    return AHC_TRUE;
}

UINT32 AHC_GetStorageTotalCluster(void)
{
	printc("m_StorageSizeInCluster %d\r\n", m_StorageSizeInCluster);
	return m_StorageSizeInCluster;
}

AHC_BOOL AHC_EnableDir(void)
{
	UINT8 i;

	for(i = 0; i < DCF_DB_COUNT; i++)
	{
		AHC_UF_EnableDB(i,AHC_TRUE);
	}
	#if (DCF_DB_COUNT >= 3)
	if(!AHC_Gsensor_Module_Attached())
	AHC_UF_EnableDB(DCF_DB_TYPE_3RD_DB, AHC_FALSE);
	#endif
	#if (SUPPORT_PARKINGMODE == PARKINGMODE_NONE)
	AHC_UF_EnableDB(DCF_DB_TYPE_2ND_DB, AHC_FALSE);
	#endif
	return AHC_TRUE;
}


__inline const FS_DEVICE_TYPE* AIHC_MediaGetDriver( AHC_FLASH_ID id )
{
    const FS_DEVICE_TYPE *pFlashDriver = NULL;    
    switch(id)
    {
        #if FS_USE_SFLASH_DRIVER
        case AHC_MEDIA_SERIAL_FLASH:  pFlashDriver = &FS_SFLASH_Driver;         break;
        #endif
        
        #if FS_USE_MMC_DRIVER
        case AHC_MEDIA_SDMMC:         pFlashDriver = &FS_MMC_CardMode_Driver;   break;
        #endif

        #if FS_USE_MMC1_DRIVER
        case AHC_MEDIA_SDMMC1:        pFlashDriver = &FS_MMC1_CardMode_Driver;  break;
        #endif

        #if FS_USE_SMC_DRIVER
        case AHC_MEDIA_NAND:          pFlashDriver = &FS_SMC_Driver;            break;
        #endif 
        
        default:
            break;
    }

    return pFlashDriver;
    
}
extern void MMPF_MMU_FlushDCache(void);

AHC_BOOL AHC_MediaRead(AHC_FLASH_ID nFlashId, ULONG ulSectorAddr, ULONG ulSectorNum, BYTE* pbyBuffer )
{
    const FS_DEVICE_TYPE *pFlashDriver = AIHC_MediaGetDriver(nFlashId);

    if( pFlashDriver == NULL )
    {
        return AHC_FALSE;
    }

    MMPF_MMU_FlushDCache();
    if( 0 != pFlashDriver->pfReadBurst( STORAGE_DIRECTLY_ACCESS_UNIT, ulSectorAddr, ulSectorNum, pbyBuffer ) )
    {
        return AHC_FALSE;
    }

    return AHC_TRUE;    
    
}

AHC_BOOL AHC_MediaWrite(AHC_FLASH_ID nFlashId, ULONG ulSectorAddr, ULONG ulSectorNum, BYTE* pbyBuffer )
{

    const FS_DEVICE_TYPE *pFlashDriver = AIHC_MediaGetDriver(nFlashId);

    if( pFlashDriver == NULL )
    {
        return AHC_FALSE;
    }

    MMPF_MMU_FlushDCache();
    if( 0 != pFlashDriver->pfWriteBurst( STORAGE_DIRECTLY_ACCESS_UNIT, ulSectorAddr, ulSectorNum, pbyBuffer ) )
    {
        return AHC_FALSE;
    }

    return AHC_TRUE;    
}

UINT32 AIHC_GetSectorCheckSum( AHC_FLASH_ID nFlashId, ULONG ulSectorAddr, BYTE* pbyBuffer )
{
    int i;
    UINT32 uiCheckSum = 0;
    UINT32* puiData   = (UINT32*)pbyBuffer;
    
    MEMSET( pbyBuffer, 0, 512 );

    if( AHC_MediaRead( nFlashId, ulSectorAddr, 1, pbyBuffer ) )
    {
        for( i=0; i<512/sizeof(UINT32); i++)
        {
            uiCheckSum += puiData[i];
        }
    }
    else
    {
        printc("Read failed %d\n", ulSectorAddr );
    }
    return uiCheckSum;
}

#if defined(UI_MIO_AMY) && (UI_MIO_AMY == 1)
#define SUPPORT_AITBOOT_EX 	(1) // TBD, moved to Config_SDK.h?

#define STORAGE_TEMP_BUFFER (0x00106000)

static ULONG byCheckSumSector[512/sizeof(ULONG)];

UINT32 AHC_Media_SerialFlashFwCheckSum(void)
{
	//int i;
	int n = 0;
	UINT32 uiCheckSum = 0;
	UINT32 ulSifAddr = 0x0;
	AIT_STORAGE_INDEX_TABLE2 *pIndexTable = (AIT_STORAGE_INDEX_TABLE2 *)STORAGE_TEMP_BUFFER;

	/*
	 * AitMiniBootloader
	 */
	//ulSifAddr = 0x0;
	printc("dbg-%s ulSifAddr[0] = 0x%X Line%d\r\n", __func__, ulSifAddr, __LINE__);
	MMPF_SF_FastReadData(ulSifAddr, STORAGE_TEMP_BUFFER, 512); // AitMiniBootloader Header Table
	{
		ULONG ulStartSec 	= pIndexTable->bt.ulStartBlock;
		ULONG ulSecSize 	= pIndexTable->bt.ulCodeSize >> 9;

		if ((PART_FLAG_VALID & pIndexTable->ulFlag) && 
			(ulStartSec) != 0 && (ulStartSec != 0xFFFFFFFF) && 
			(ulSecSize) != 0 && (ulSecSize  != 0xFFFFFFFF))
		{
			int j;

			for (j = ulStartSec; j < (ulStartSec + ulSecSize); j++)
			{
				uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, j, (BYTE*)byCheckSumSector);
				n++;
			}
			printc("CheckSum AitMiniBootloader 0x%X %d-%d\r\n", uiCheckSum, ulStartSec, ulSecSize);
		}
	}

	/*
	 * AitBootloader
	 */
	ulSifAddr = ulSifAddr + (0x1 << 12) * (pIndexTable->ulTotalSectorsInLayer);
	printc("dbg-%s ulSifAddr[1] = 0x%X Line%d\r\n", __func__, ulSifAddr, __LINE__);
	MMPF_SF_ReadData(ulSifAddr, STORAGE_TEMP_BUFFER, 512); // AitBootloader Header Table
	{
		ULONG ulStartSec 	= pIndexTable->bt.ulStartBlock;
		ULONG ulSecSize 	= pIndexTable->bt.ulCodeSize >> 9;

		if ((PART_FLAG_VALID & pIndexTable->ulFlag) && 
			(ulStartSec) != 0 && (ulStartSec != 0xFFFFFFFF) && 
			(ulSecSize) != 0 && (ulSecSize  != 0xFFFFFFFF))
		{
			int j;

			for (j = ulStartSec; j < (ulStartSec + ulSecSize); j++)
			{
				uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, j, (BYTE*)byCheckSumSector);
				n++;
			}
			printc("CheckSum AitBootloader 0x%X %d-%d\n", uiCheckSum, ulStartSec, ulSecSize);
		}
	}

#if SUPPORT_AITBOOT_EX
	/*
	 * AitBootloaderEX
	 */
	ulSifAddr = ulSifAddr + (0x1 << 12) * (pIndexTable->ulTotalSectorsInLayer);
	printc("dbg-%s ulSifAddr[2] = 0x%X Line%d\r\n", __func__, ulSifAddr, __LINE__);
	MMPF_SF_ReadData(ulSifAddr, STORAGE_TEMP_BUFFER, 512); // AitBootloaderEX Header Table
	{
		ULONG ulStartSec 	= pIndexTable->bt.ulStartBlock;
		ULONG ulSecSize 	= pIndexTable->bt.ulCodeSize >> 9;

		if ((PART_FLAG_VALID & pIndexTable->ulFlag) && 
			(ulStartSec) != 0 && (ulStartSec != 0xFFFFFFFF) && 
			(ulSecSize) != 0 && (ulSecSize  != 0xFFFFFFFF))
		{
			int j;

			for (j = ulStartSec; j < (ulStartSec + ulSecSize); j++)
			{
				uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, j, (BYTE*)byCheckSumSector);
				n++;
			}
			printc("CheckSum AitBootloaderEX 0x%X %d-%d\r\n", uiCheckSum, ulStartSec, ulSecSize);
		}
	}
#endif

	/*
	 * AitALL
	 */
	ulSifAddr = ulSifAddr + (0x1 << 12) * (pIndexTable->ulTotalSectorsInLayer);
	printc("dbg-%s ulSifAddr[3] = 0x%X Line%d\r\n", __func__, ulSifAddr, __LINE__);
	MMPF_SF_ReadData(ulSifAddr, STORAGE_TEMP_BUFFER, 512); // AitALL Header Table
#if 1 // TBD
	uiCheckSum = 0;
#endif
	{
		ULONG ulStartSec 	= pIndexTable->bt.ulStartBlock;
		ULONG ulSecSize 	= pIndexTable->bt.ulCodeSize >> 9;

		if ((PART_FLAG_VALID & pIndexTable->ulFlag) && 
			(ulStartSec) != 0 && (ulStartSec != 0xFFFFFFFF) && 
			(ulSecSize) != 0 && (ulSecSize  != 0xFFFFFFFF))
		{
			int j;

			for (j = ulStartSec; j < (ulStartSec + ulSecSize); j++)
			{
				uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, j, (BYTE*)byCheckSumSector);
				n++;
			}
			printc("CheckSum AitALL 0%X %d-%d\r\n", uiCheckSum, ulStartSec, ulSecSize);
		}
	}

	printc("CheckSum caculate sectors number %d\n", n);

	return uiCheckSum;
}
#else
/*
 * old for AIT8427 platform
 */
// Define SEC_BOOT2 boot code position and size
#define SEC_BOOT_START      0
#define SEC_BOOT_SIZE       32

static ULONG byCheckSumSector[512/sizeof(ULONG)];
static ULONG byBootSector[512/sizeof(ULONG)];

UINT32 AHC_Media_SerialFlashFwCheckSum(void)
{
    int i;
    int n = 0;
    UINT32 uiCheckSum = 0;
    //AIT_STORAGE_INDEX_TABLE * pAITStorInfo;
	AIT_STORAGE_INDEX_TABLE2 *pIndexTable;

    // 1. SEC_BOOT2 16KB
    for( i=SEC_BOOT_START; i<SEC_BOOT_SIZE; i++ )
    {
        uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, i, (BYTE*) byCheckSumSector);

        printc("BOOT CheckSum[%d]  %x\n", i, uiCheckSum );        
        n++;
    }

    printc("1. CheckSum  %x\n", uiCheckSum );

    
    // 2. BOOT SECTOR (Partition table)
    uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, SF_BOOT_OFFSET, (BYTE*)byBootSector);
    printc("2. CheckSum  %x\n", uiCheckSum );
    n++;

    // 3. AIT_BOOT
	//pAITStorInfo = (AIT_STORAGE_INDEX_TABLE *)byBootSector;
	pIndexTable = (AIT_STORAGE_INDEX_TABLE2 *)byBootSector;
	

    //for(i=0; i< 10; i++ )
    {
        ULONG ulStartSec = pIndexTable->bt.ulStartBlock/*pAITStorInfo->it[i].ulStartSec*/;
        ULONG ulSecSize  = pIndexTable->bt.ulCodeSize/*pAITStorInfo->it[i].ulSecSize*/;
        
        if( (PART_FLAG_VALID & pIndexTable->ulFlag/*pAITStorInfo->it[i].ulFlag0*/ ) && 
             ulStartSec != 0 && ulStartSec != 0xFFFFFFFF     &&
             ulSecSize != 0  && ulSecSize  != 0xFFFFFFFF        )
        {
            
            int j;
            for( j = ulStartSec; j< ulStartSec + ulSecSize ; j++ )
            {
                uiCheckSum += AIHC_GetSectorCheckSum(AHC_MEDIA_SERIAL_FLASH, j, (BYTE*)byCheckSumSector);
                n++;
            }
            printc("CheckSum[%d]  %x %d-%d\n",i, uiCheckSum, ulStartSec, ulSecSize );

        }
    }

    printc("CheckSum caculate sectors number %d\n", n );

    return uiCheckSum;
}
#endif

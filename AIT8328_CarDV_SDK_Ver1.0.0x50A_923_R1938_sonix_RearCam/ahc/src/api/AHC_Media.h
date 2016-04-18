//==============================================================================
//
//  File        : AHC_Media.h
//  Description : INCLUDE File for the AHC Media function porting.
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#ifndef _AHC_MEDIA_H_
#define _AHC_MEDIA_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_FS.h"
#include "MenuSetting.h"

/*===========================================================================
 * Enum define
 *===========================================================================*/ 
 
typedef enum _AHC_MEDIA_ATTR {
	AHC_MEDIA_ATTR_WP = 0,
	AHC_MEDIA_ATTR_VOL_ID,
	AHC_MEDIA_ATTR_TYPE,
	AHC_MEDIA_ATTR_STATUS
} AHC_MEDIA_ATTR;

typedef enum _AHC_MEDIA_ID {
    AHC_MEDIA_MMC = 0,
    AHC_MEDIA_MMC1,
    AHC_MEDIA_SMC,
    AHC_MEDIA_MEMDISK,
    AHC_MEDIA_RESOURCE_FAT,
    AHC_MEDIA_FACTORY_FAT,
    AHC_MEDIA_USER_FAT,
    AHC_MEDIA_MAX_ID
} AHC_MEDIA_ID;

// Definition of Flash Storage
typedef enum _AHC_FLASH_ID {
    AHC_MEDIA_SERIAL_FLASH = 0,
    AHC_MEDIA_SDMMC,
    AHC_MEDIA_SDMMC1,
    AHC_MEDIA_NAND
} AHC_FLASH_ID;

//MMOUNT:	Mount as Master
//NMOUNT:	Not Inserted
//SMOUNT:	Mount as Slave
typedef enum _AHC_SD_MOUNT_STATUS_ {
	AHC_SD1_NO_MOUNT_SD2_NO_MOUNT = 0,//No Card
	AHC_SD1_MASTER_MOUNT_SD2_NO_MOUNT,//Only SD1 inserted
	AHC_SD1_MASTER_MOUNT_SD2_SLAVE_MOUNT,//SD1 acts as Master, SD2 acts as Slave
	AHC_SD1_NO_MOUNT_SD2_MASTER_MOUNT,//Only SD2 inserted
	AHC_SD1_SLAVE_MOUNT_SD2_MASTER_MOUNT//SD1 acts as Slave, SD2 acts as Master
	
}AHC_SD_MOUNT_STATUS;
/*===========================================================================
 * Function prototype
 *===========================================================================*/ 
 
AHC_BOOL AHC_GetImageDateTime(UINT16 *puwYear, UINT8 *puwMonth, UINT8 *puwDay, UINT8 *puwHour, UINT8 *puwMin, UINT8 *puwSec);
AHC_BOOL AHC_FormatStorageMedia(UINT8 byMediaID);
#if (FS_FORMAT_FREE_ENABLE)
AHC_BOOL AHC_FormatStorageMedia_FormatFree( UINT8 byMediaID );
AHC_BOOL AHC_CheckMedia_FormatFree( UINT8 byMediaID );
#endif
AHC_BOOL AHC_MountStorageMedia(UINT8 byMediaID);
AHC_BOOL AHC_MountStorageMediaWithoutDCF(UINT8 byMediaID);
AHC_BOOL AHC_DisMountStorageMedia(UINT8 byMediaID);
AHC_BOOL AHC_DisMountStorageMediaOnlyPhysical(UINT8 byMediaID);
AHC_BOOL AHC_GetMountStorageMediaStatus(void);
//AHC_BOOL AHC_GetStorageMediaStatus(UINT16 *pwImageCount, UINT16 *pwImageLeft, UINT64 *pullFreeSize);
AHC_BOOL AHC_GetStorageMediaStatus(void);
AHC_BOOL AHC_GetStorageOtherMediaStatus(UINT8 byMediaID, UINT64 *pullFreeSize);
AHC_BOOL AHC_GetStorageMediaGeometry(UINT16 *pwBytesPerSec, UINT16 *pwSecPerCluster, UINT32 *pwTotalCluster, UINT32 *pwFreelCluster);
AHC_BOOL AHC_GetStorageMediaInfo(AHC_FS_DISK_INFO* sMediaInfo);
AHC_BOOL AHC_SetMediaAttr(UINT8 byMediaID, AHC_MEDIA_ATTR bMediaAttr, UINT32 uiParam);
AHC_BOOL AHC_GetMediaAttr(UINT8 byMediaID, AHC_MEDIA_ATTR bMediaAttr, UINT32 *puiParam);
AHC_BOOL AHC_SetMediaPath(AHC_MEDIA_ID path);
AHC_MEDIA_ID AHC_GetMediaPath(void);
AHC_BOOL AHC_SwitchMediaPath(void);
AHC_BOOL AHC_RemountDevices(MEDIASETING_SETTING media);
AHC_BOOL AHC_IsSDInserted(void);
AHC_BOOL AHC_Is2ndSDInserted(void);
AHC_BOOL AHC_IsSDWriteProtect(void);
AHC_BOOL AHC_SetClearCachePeriod(UINT8 byMediaID, UINT32 ulSize);
AHC_BOOL AHC_MountPhysicalDevice(UINT8 byMediaID);
AHC_BOOL AHC_UnmountPhysicalDevice(UINT8 byMediaID);
AHC_BOOL AHC_DisMountStorageMediaEx(UINT8 byMediaID, AHC_BOOL bUnmountPhysical );
AHC_BOOL AHC_UnmountVolume(UINT8 byMediaID);
AHC_BOOL AHC_ResetPhysicalDevice(UINT8 byMediaID);
AHC_BOOL AHC_GetVolumeName(INT8* pchVolume, UINT32* puiVolNameSize);
AHC_BOOL AHC_EnableDir(void);
UINT32   AHC_Media_SerialFlashFwCheckSum( void );

#endif //_AHC_MEDIA_H_

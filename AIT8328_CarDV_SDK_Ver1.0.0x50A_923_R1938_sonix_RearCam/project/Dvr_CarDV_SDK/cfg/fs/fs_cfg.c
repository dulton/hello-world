//==============================================================================
//
//  File        : FS_cfg.c
//  Description : File System configure file
//  Author      : Rony Yeh
//  Revision    : 1.0
//==============================================================================

#include "FS_cfg.h"

//==============================================================================
//
//                              EXTERNAL FUNC
//
//==============================================================================

extern void _SD_DevSetPad(MMP_ULONG ulPad);

//==============================================================================
//
//                              Structure
//
//==============================================================================

typedef struct _FCTABLE_LIST 
{
    MMP_UBYTE *ubTableAddress;
	MMP_ULONG  ulTableLength;
}FCTABLE_LIST;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

#if (FS_SUPPORT_CACHE)
//__align(8)        MMP_ULONG acCache[2];	//Ted remove after verifying all function
#if (FS_MULTI_CACHE)
__align(8)        MMP_ULONG acCacheMemory[FS_MAX_UNITS][(FS_CACHE_LENGTH>>2)*3];
#else
__align(8)        MMP_ULONG acCacheMemory[FS_MAX_UNITS][FS_CACHE_LENGTH>>2];
#endif
#endif

#if (FS_NEW_GET_FREE_SPACE == 1)||(FS_SUPPORT_FAST_FORMAT == 1)
//#define	SD_GET_FREE_SPACE_SIZE	(8*1024)
//__align(32)		MMP_UBYTE m_ubSDGetFreeSpaceTmp[SD_GET_FREE_SPACE_SIZE];
//extern void MMPF_MMU_InvalidateDCacheMVA(MMP_ULONG ulRegion, MMP_ULONG ulSize);
//extern void MMPF_MMU_FlushDCacheMVA(MMP_ULONG ulRegion, MMP_ULONG ulSize);
#if (FS_NEW_GET_FREE_SPACE == 1)&&(FS_NEW_SEARCH_FREE_CLUSTER == 1)
MMP_UBYTE m_ubSDFreeClusterTable1[FS_1ST_DEV_FREE_CLUSTER_TABLE_SIZE];
#if (FS_MAX_UNITS > 1)
MMP_UBYTE m_ubSDFreeClusterTable2[FS_2ND_DEV_FREE_CLUSTER_TABLE_SIZE];
#endif
#if (FS_MAX_UNITS > 2)
MMP_UBYTE m_ubSDFreeClusterTable3[FS_3RD_DEV_FREE_CLUSTER_TABLE_SIZE];
#endif
#if (FS_MAX_UNITS > 3)
MMP_UBYTE m_ubSDFreeClusterTable4[FS_4TH_DEV_FREE_CLUSTER_TABLE_SIZE];	
#endif
#if (FS_MAX_UNITS > 4)
MMP_UBYTE m_ubSDFreeClusterTable5[FS_5TH_DEV_FREE_CLUSTER_TABLE_SIZE];
#endif

MMP_UBYTE *m_ubSDFreeClusterTable[FS_MAX_UNITS] = {
    m_ubSDFreeClusterTable1,
#if (FS_MAX_UNITS > 1)
    m_ubSDFreeClusterTable2,
#endif
#if (FS_MAX_UNITS > 2)
    m_ubSDFreeClusterTable3,
#endif
#if (FS_MAX_UNITS > 3)
    m_ubSDFreeClusterTable4,
#endif
#if (FS_MAX_UNITS > 4)
    m_ubSDFreeClusterTable5,
#endif
};

FCTABLE_LIST FCTableContent[FS_MAX_UNITS] = {
    {m_ubSDFreeClusterTable1,FS_1ST_DEV_FREE_CLUSTER_TABLE_SIZE},
#if (FS_MAX_UNITS > 1)    
    {m_ubSDFreeClusterTable2,FS_2ND_DEV_FREE_CLUSTER_TABLE_SIZE},
#endif
#if (FS_MAX_UNITS > 2)  
    {m_ubSDFreeClusterTable3,FS_3RD_DEV_FREE_CLUSTER_TABLE_SIZE},
#endif
#if (FS_MAX_UNITS > 3)    
    {m_ubSDFreeClusterTable4,FS_4TH_DEV_FREE_CLUSTER_TABLE_SIZE},
#endif
#if (FS_MAX_UNITS > 4)    
    {m_ubSDFreeClusterTable5,FS_5TH_DEV_FREE_CLUSTER_TABLE_SIZE},
#endif
};

#endif
#endif

/* gpFS_Device are devices AIT FileSystem support */
STORAGE_UNIT gpFS_DeviceCustom[FS_MAX_UNITS] = 
{
    {FS_1ST_DEV, 0, NULL}, 
    #if (FS_MAX_UNITS > 1)
    {FS_2ND_DEV, 0, NULL}, 
    #endif
    #if (FS_MAX_UNITS > 2)
    {FS_3RD_DEV, 0, NULL}, 
    #endif
    #if (FS_MAX_UNITS > 3)
    {FS_4TH_DEV, 0, NULL}, 
    #endif
    #if (FS_MAX_UNITS > 4)
    {FS_5TH_DEV, 0, NULL}, 
    #endif
};

/* gpMSDC_Device are devices AIT MSDC support */
STORAGE_UNIT gpMSDC_DeviceCustom[MSDC_MAX_UNITS] = 
{
    {MSDC_1ST_DEV, 0, NULL}, 
#if (MSDC_MAX_UNITS > 1)
    {MSDC_2ND_DEV, 0, NULL}, 
#endif
#if (MSDC_MAX_UNITS > 2)
    {MSDC_3RD_DEV, 0, NULL}, 
#endif
#if (MSDC_MAX_UNITS > 3)
    {MSDC_4TH_DEV, 0, NULL}, 
#endif
#if (MSDC_MAX_UNITS > 4)
    {MSDC_5TH_DEV, 0, NULL},
#endif
};

int FS_Module_Init(void)
{
    MMP_ULONG dev;

    _SD_DevSetPad(SD_SOCKET0_MAP_ID);

    for (dev = 0; dev < FS_MAX_UNITS; dev++) {
        MMPF_STORAGE_RegisterFSDevie(&gpFS_DeviceCustom[dev],
                                    (MMP_ULONG)&acCacheMemory[dev],
                                    FS_CACHE_LENGTH,
                                    (MMP_ULONG)(m_ubSDFreeClusterTable[dev]),
                                    FCTableContent[dev].ulTableLength); 
    }

    for (dev = 0; dev < MSDC_MAX_UNITS; dev++) {
        MMPF_STORAGE_RegisterMSDCDevie(&gpMSDC_DeviceCustom[dev]);
    }

    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6" 
#pragma O0
ait_module_init(FS_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata


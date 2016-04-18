//==============================================================================
//
//  File        : mmpf_cpu.c
//  Description : MMPF_SYS_subCPU functions
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================
#include "includes_fw.h"
#include "lib_retina.h"
#include "mmu.h"
#include "mmpf_cpu.h"
#include "cpucomm.h"
#include "cpucomm_bus.h"
#include "dualcpu_sysflag.h"
#include "Mmpf_fs_api.h"

#if (CHIP == MCR_V2)

//==============================================================================
//
//                              Definitions
//
//==============================================================================
typedef struct _RegionData
{
    const char  *pszFilePath;
    MMP_UBYTE   *pbyTargetAddr;
    MMP_ULONG   ulTargetSize;
    MMP_UBYTE   *pybTempAddr;
} RegionData;

//==============================================================================
//
//                              Static Variables
//
//==============================================================================
/* Firwmare binary for slave CPU */
/*
 * Firmware (in DRAM area) of slave core is built & linked with the physical
 * address as loading address of master core firmware, thus master core doesn't
 * need to download code (in DRAM area) for slave core.
 *
 * Please check scatter file of master core, the execution address of section
 * "CPUB_FW" must be identical to the execution address specified in the
 * scatter file of slave core firmware.
 */
#if (SCORE_EN)
#pragma arm section rodata = "CPUB_FW"
#include "CPUB_FW_DRAM.h"
#pragma arm section rodata
#include "CPUB_FW_ITCM.h"
#else
const MMP_ULONG CPUB_FW_DRAM[] = {0x00};
const MMP_ULONG CPUB_FW_ITCM[] = {0x00};
#endif

static RegionData sUcos2Regions[] =
{
    /* Load slave core's firmware (ITCM area) to address of its ITCM */
    {NULL, (MMP_UBYTE *)CPUB_ITCM_ADDR, sizeof(CPUB_FW_ITCM), (MMP_UBYTE *)CPUB_FW_ITCM},
    /* Slave core's firmware (DRAM area) is already loaded in master core startup */
    {NULL, (MMP_UBYTE *)CPUB_FW_DRAM,   sizeof(CPUB_FW_DRAM), (MMP_UBYTE *)CPUB_FW_DRAM}
};

static RegionData sLinuxRegions[] =
{
    { CPUB_ITCM_FILE_LINUX, (MMP_UBYTE *)CPUB_ITCM_ADDR,          CPUB_ITCM_SIZE,         (MMP_UBYTE *)CPUB_DRAM_ADDR_LINUX },
#ifdef _USE_INITRD_ // use initrd as root fs
    { CPUB_RDSK_FILE_LINUX, (MMP_UBYTE *)CPUB_RDSK_ADDR_LINUX,    CPUB_RDSK_SIZE_LINUX,   (MMP_UBYTE *)NULL },
#endif
    { CPUB_DRAM_FILE_LINUX, (MMP_UBYTE *)CPUB_ZIMG_ADDR_LINUX,    CPUB_DRAM_SIZE_LINUX,   (MMP_UBYTE *)NULL }
};

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_ReadCodeRegion
//  Description : Load image from storage to memory
//------------------------------------------------------------------------------
static MMP_ULONG MMPF_SYS_ReadCodeRegion(RegionData *psRegionData)
{
    MMP_ULONG   ulFileId = 0, ulReadSize;
    MMP_ULONG64 ulFileSize;
    MMP_ERR     ulRet;

    if (psRegionData->pszFilePath) {
        // Open file
    	if (MMPF_FS_FOpen(psRegionData->pszFilePath, "rb", &ulFileId)) {
            RTNA_DBG_Str(0, "Can't open file\r\n");
            return 0;
        }

        // Check file size
        ulRet = MMPF_FS_GetFileSize(ulFileId, &ulFileSize);
        if ((ulRet != MMP_ERR_NONE) || (ulFileSize > psRegionData->ulTargetSize)) {
            RTNA_DBG_Str(0, "File size is too big\r\n" );
            ulReadSize = 0;
            goto RETURN;
        }

        // Read code to RAM
        ulRet = MMPF_FS_FRead(ulFileId,
                              (psRegionData->pybTempAddr != NULL) ? 
                               psRegionData->pybTempAddr :
                               psRegionData->pbyTargetAddr,
                              ulFileSize,
                              &ulReadSize);
        if (ulRet != MMP_ERR_NONE) {
            RTNA_DBG_Str(0, "Can't read file\r\n");
            ulReadSize = 0;
            goto RETURN;
        }
    }
    else {
        ulReadSize = psRegionData->ulTargetSize;
    }

    // Copy data from temproal buffer to target address
    // We need this setp because some HW can't access some memory, such ash ITCM
    if ((psRegionData->pybTempAddr != NULL) &&
        (psRegionData->pbyTargetAddr != psRegionData->pybTempAddr))
    {
         MEMCPY(psRegionData->pbyTargetAddr, psRegionData->pybTempAddr, ulReadSize);
    }

RETURN:

    // Close file
    if (ulFileId)
        MMPF_FS_FClose(ulFileId);

    return ulReadSize;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_StartCpuB
//  Description :
//------------------------------------------------------------------------------
/** @brief The function is used to download and run sub cpu
 function is used to download and run sub cpu
@param[in] ulAllDramStAddr is used to set FW ALL_DRAM's address
@return MMP_ERR_NONE,MMP_SYSTEM_ERR_CPUBOOT
*/
MMP_ERR MMPF_SYS_StartCpuB(RegionData* psRegionData, MMP_ULONG ulRegionData)
{

    AITPS_CORE  pCORE = AITC_BASE_CORE;
    MMP_ULONG   ulFileSize, i;

    //Enable CPUB clock
    pCORE->CORE_B_WD_CFG |= HOST_RST_CPU_EN;
    //Enable CPUA to CPUB download mode
    pCORE->CORE_MISC_CFG &=~ (CPU_A2B_DNLD_EN | CPU_B2A_DNLD_EN);
    pCORE->CORE_MISC_CFG |= CPU_A2B_DNLD_EN;
    //Hang CPUB
    pCORE->CORE_B_CFG |= CPU_RST_EN;

    // Init CpuComm service on CPU A side at first
    if (CpuComm_Init() != CPU_COMM_ERR_NONE) {
        RTNA_DBG_Str(0, "can't init comm module\r\n");
        return MMP_SYSTEM_ERR_CPUBOOT;
    }

    for(i = 0; i < ulRegionData; i++) {
        ulFileSize = MMPF_SYS_ReadCodeRegion(psRegionData+i);
        if (ulFileSize == 0) {
            RTNA_DBG_Str(0, "Can't load FW\r\n");
            return MMP_SYSTEM_ERR_CPUBOOT;
        }
    }

    // Clean entire cache.
    // The code size of CPU B is too huge so clean entire cache is more efficiant
    MMPF_MMU_CleanDCache();

    //Release CPUB
    pCORE->CORE_B_WD_CFG &= ~HOST_RST_CPU_EN;
    pCORE->CORE_B_CFG &=~ CPU_RST_EN;

    CpuComm_Start();

    // Wait CPU B init done
    DualCpu_SysFlag_WaitCpuBInitDone();

    // MMPF_OS_SleepMs( 10000 );

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_SYS_StopCpuB
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_SYS_StopCpuB(void)
{
    AITPS_CORE  pCORE = AITC_BASE_CORE;
#if 0
    // Notify CPU B to terminate itself
    DualCpu_SysFlag_TerminateCpuB();
#endif

    pCORE->CORE_B_WD_CFG |= HOST_RST_CPU_EN;
    pCORE->CORE_B_CFG |= CPU_RST_EN;
    pCORE->CORE_B_WD_CFG &= ~HOST_RST_CPU_EN;

    // Destroy CpuComm service on CPU A
    CpuComm_Destroy();

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Sys_StartCpuB_Ucos2
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_SYS_StartCpuB_Ucos2(void)
{
    return MMPF_SYS_StartCpuB(sUcos2Regions, sizeof(sUcos2Regions)/sizeof(RegionData));
}

//------------------------------------------------------------------------------
//  Function    : MMPF_Sys_StartCpuB_Linux
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_SYS_StartCpuB_Linux(void)
{
    return MMPF_SYS_StartCpuB(sLinuxRegions, sizeof(sLinuxRegions)/sizeof(RegionData));
}

#if 0

#define ATAG_NONE	    0x00000000
#define ATAG_CORE	    0x54410001
#define ATAG_MEM	    0x54410002
#define ATAG_CMDLINE	0x54410009

typedef struct _AtagHeader {
    unsigned long ulSize;
    unsigned long ulTag;
} AtagHeader;

typedef struct _AtagCore {
    unsigned long ulFlags;		/* bit 0 = read-only */
    unsigned long ulPageSize;
    unsigned long ulRootDev;
} AtagCore;

typedef struct _AtagMem {
    unsigned long ulSize;
    unsigned long ulStart;	/* physical start address */
} AtagMem;

#define _STRING(str) #str
#define STRING(str)  _STRING(str)

char pszLinuxAtagComd[] = "mem="STRING(CPUB_DRAM_SIZE_LINUX_MB)"M console=ttyS0,115200 initrd=0x2200000,18M init=/init root=/dev/ram0 mtdparts=spi0.0:1M(boot)ro,2M(kernel)ro,8M(root.ext2)ro,1M(jffs2),-(appfs);";

static void boot_boot_show_params( unsigned char *pbyStart, unsigned char *pbyEnd )
{
    printc( "from %08lx to %08lx\r\n", (unsigned long)pbyStart, (unsigned long)pbyEnd );

    for( ; pbyStart < pbyEnd; pbyStart += 16 )
    {
        // printc( "%08lx %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n", pbyStart,
        printc( "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",
                pbyStart[0], pbyStart[1], pbyStart[2], pbyStart[3], pbyStart[4], pbyStart[5], pbyStart[6], pbyStart[7],
                pbyStart[8], pbyStart[9], pbyStart[10], pbyStart[11], pbyStart[12], pbyStart[13], pbyStart[14], pbyStart[15] );
    }
}


//------------------------------------------------------------------------------
//  Function    : CpuBSysStartup
//  Description : This is the SYS startup task
//------------------------------------------------------------------------------
static void CreateLinuxAtag( unsigned char *pbyBuffer )
{
    AtagHeader  *psHeader;
    AtagCore    *psCore;
    AtagMem     *psMem;

    // Clean data
    memset( pbyBuffer, 0, 0x100 );

    printc( "pszLinuxAtagComd = %d %d(%c)\r\n", sizeof(pszLinuxAtagComd), pszLinuxAtagComd[sizeof(pszLinuxAtagComd)-2], pszLinuxAtagComd[sizeof(pszLinuxAtagComd)-2] );
    printc( "%s\r\n", pszLinuxAtagComd );

    // ATAG_CORE
    psHeader = (AtagHeader*)pbyBuffer;
    psHeader->ulTag  = ATAG_CORE;
    psHeader->ulSize = ( sizeof(AtagHeader) + sizeof(AtagCore) ) >> 2;
    psCore = (AtagCore*)(psHeader+1);
    psCore->ulFlags = psCore->ulPageSize = psCore->ulRootDev = 0;

    // ATAG_CMDLINE
    psHeader = (AtagHeader*)(psCore+1);
    psHeader->ulTag  = ATAG_CMDLINE;
    psHeader->ulSize = ( sizeof(AtagHeader) + sizeof(pszLinuxAtagComd) + 3 ) >> 2;
    memcpy( (void*)(psHeader+1), pszLinuxAtagComd, sizeof(pszLinuxAtagComd)+1 );

    // ATAG_MEM
    psHeader = (AtagHeader*)( (unsigned char*)psHeader + (psHeader->ulSize << 2) );
    psHeader->ulTag  = ATAG_MEM;
    psHeader->ulSize = ( sizeof(AtagHeader) + sizeof(AtagMem) ) >> 2;
    psMem = (AtagMem*)(psHeader+1);
    // Linux use the 32M memory at the bottom of DRAM.
    psMem->ulStart = 0x03000000;
    psMem->ulSize  = 0x02000000;

    // ATAG_NONE
    psHeader = (AtagHeader*)(psMem+1);
    psHeader->ulTag  = ATAG_NONE;
    psHeader->ulSize = 0;

    boot_boot_show_params( pbyBuffer, (unsigned char *)(psHeader+1) );
}

#endif




#if 0
#include "CpuIrqTest.c"
#endif

#endif



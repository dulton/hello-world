//==============================================================================
//
//  File        : JobDispatch.c
//  Description : ce job dispatch function
//  Author      : Robin Feng
//  Revision    : 1.0
//
//==============================================================================
#include "Customer_Config.h"

#include "includes_fw.h"
#include "mmpf_typedef.h"

#include "bootloader.h"		// CarDV

#include "mmp_reg_gpio.h"
#include "mmp_reg_gbl.h"
#include "mmp_reg_dma.h"
#include "mmp_reg_sd.h"

#include "os_wrap.h"
#include "mmph_hif.h"
#include "mmpd_display.h"
#include "mmps_dsc.h"
#include "mmps_fs.h"
#include "mmps_system.h"
#include "mmpd_system.h"
#include "mmpf_dram.h"
#include "mmpf_fs_api.h"
#include "mmps_sensor.h"
#include "mmps_usb.h"
#include "mmps_dsc.h"
#include "mmps_audio.h"
#include "mmps_vidplay.h"
#include "mmpf_uart.h"
#include "mmpf_pll.h"
#include "mmpf_storage_api.h"
#include "mmpf_sd.h"
#include "mmpf_hif.h"
#include "mmpf_mci.h"
#include "mmpf_system.h"
#include "mmpf_display.h"
#include "ait_config.h"
#include "mmpf_pio.h"
#include "mmpf_adc.h"
#include "mmpf_i2cm.h"
#include "mmpf_dma.h"
#include "Clk_cfg.h"
#if (USING_SM_IF == 1)
#include "mmpf_nand.h"
#endif
#include "PMUCtrl.h"        // CarDV
#include "fs_cfg.h" // CarDV

//==============================================================================
//
//                              COMPILING OPTIONS
//
//==============================================================================

#if defined(SD_BOOT)
#define BOOT_FROM_SD        (1)
#define BOOT_FROM_SF        (0)
#else
#define BOOT_FROM_SD        (0)
#define BOOT_FROM_SF        (1)
#endif

#define BOOT_FROM_NAND      (0)

#if (BOOT_FROM_SF)
#include "mmpf_sf.h"
#endif

#ifndef JTAG_EN             // maybe defined in ADS
#define JTAG_EN             (1)
#endif
// When JTAG_EN is 1
// JTAG_TIME_OUT is 0 - go to JTAG immediately
//               is S - pause, and wait any key from UART in S seconds into JTAG.
//                      resume, if no any after S seconds.
#define	JTAG_TIME_OUT		(1)

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
extern void Customer_Init_CarDV(void);

//#define DRAM_TEST

#ifdef DRAM_TEST
volatile MMP_UBYTE DMADone = 0;
void ReleaseDma(void *argu)
{
	RTNA_DBG_Str(0, "ReleaseDma\r\n");
	DMADone = 1;
}

MMP_ERR DramTest1 (MMP_ULONG size, MMP_ULONG offset)     
{	
	MMP_ULONG *ulTempAddr = (MMP_ULONG *)0x1000000;
    MMP_ULONG i;
    for (i = 0 ; i < size; i+=offset) {
    	*(ulTempAddr +i)= i;
    }
    for (i = 0 ; i < size; i+=offset) {
    	if (*(ulTempAddr +i) != i) {
    		RTNA_DBG_Str(0, "Dram test fail 1:");
    		RTNA_DBG_Long(0, *(ulTempAddr +i));
    		RTNA_DBG_Str(0, ":");
    		RTNA_DBG_Long(0, i);
    		RTNA_DBG_Str(0, "\r\n");
    		//return MMP_SYSTEM_ERR_HW;
    	}
    }
    for (i = 0 ; i < size; i+=offset) {
    	*(ulTempAddr +i)= 0xFFFFFFFF - i;
    }
    for (i = 0 ; i < size; i+=offset) {
    	if (*(ulTempAddr +i) != 0xFFFFFFFF - i) {
    		RTNA_DBG_Str(0, "Dram test fail 2:");
    		RTNA_DBG_Long(0, *(ulTempAddr +i));
    		RTNA_DBG_Str(0, ":");
    		RTNA_DBG_Long(0, 0xFFFFFFFF-i);
    		RTNA_DBG_Str(0, "\r\n");
    		//return MMP_SYSTEM_ERR_HW;
    	}
    }
    return MMP_ERR_NONE;
}

MMP_ERR DramDMATest(MMP_ULONG src, MMP_ULONG dst, MMP_ULONG wordSize, MMP_USHORT usLoop)     
{
    MMP_ULONG   max_transfer;
    MMP_USHORT  *dev_src = (MMP_USHORT *)src;
    MMP_USHORT  *dev_dst = (MMP_USHORT *)dst;
    MMP_LONG	i;
    MMP_ULONG   loop = 0;
    MMP_USHORT	ret = 0;
    MMP_ULONG   fail_cnt = 0;
    
    MMP_GRAPHICS_BUF_ATTR 	srcbufattribute;
    MMP_GRAPHICS_RECT 		srcrect;
    MMP_GRAPHICS_BUF_ATTR 	dstbufattribute;

    if (wordSize > 0x20000) //(height - 1) can't over 12-bit
        wordSize = 0x20000;
    max_transfer = wordSize;

    srcbufattribute.usWidth = 16;
    srcbufattribute.usHeight = max_transfer >> 4;
    srcbufattribute.usLineOffset = 32;
    srcbufattribute.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
    srcbufattribute.ulBaseAddr = (MMP_ULONG)dev_src;
    srcbufattribute.ulBaseUAddr = 0;
    srcbufattribute.ulBaseVAddr = 0;

    srcrect.usLeft = 0;
    srcrect.usTop = 0;
    srcrect.usWidth = srcbufattribute.usWidth;
    srcrect.usHeight = srcbufattribute.usHeight;

    dstbufattribute.usWidth = srcbufattribute.usWidth;
    dstbufattribute.usHeight = srcbufattribute.usHeight;
    dstbufattribute.usLineOffset = 32;
    dstbufattribute.colordepth = MMP_GRAPHICS_COLORDEPTH_16;
    dstbufattribute.ulBaseAddr = (MMP_ULONG)dev_dst;
    dstbufattribute.ulBaseUAddr = 0;
    dstbufattribute.ulBaseVAddr = 0;

    MMPF_DMA_Initialize();
    while (1) {
        if (loop == usLoop)
            break;

        loop++;

        RTNA_DBG_Str(0, "Loop test:");
        RTNA_DBG_Long(0, loop);
        RTNA_DBG_Str(0, "\r\n");
        fail_cnt = 0;

        srand(loop);
        for (i = 0 ; i < max_transfer; i++) {
            *(dev_src +i) = RAND() % 0xFFFF;
        }

        RTNA_DBG_Str(0, "Test:");
        RTNA_DBG_Long(0, (MMP_ULONG)dev_src);
        RTNA_DBG_Str(0, ":");
        RTNA_DBG_Long(0, (MMP_ULONG)dev_dst);
        RTNA_DBG_Str(0, ":");
        RTNA_DBG_Long(0, (MMP_ULONG)max_transfer);
        RTNA_DBG_Str(0, "\r\n");   
        DMADone = 0;
        MMPF_DMA_RotateImageBuftoBuf(&srcbufattribute, &srcrect, 
                                     &dstbufattribute, 0, 0, 
                                     MMP_GRAPHICS_ROTATE_NO_ROTATE, 
    	                             ReleaseDma, 0, MMP_FALSE, 2);
        while(DMADone == 0);

        for (i = 0; i < max_transfer; i++) {
            if (*(dev_src+i) != *(dev_dst+i)) {
                RTNA_DBG_Str(0, "Compare fail:");
                RTNA_DBG_Str(0, " ~");
                RTNA_DBG_Long(0, (MMP_ULONG)dev_src+i);
                RTNA_DBG_Str(0, ":");
                RTNA_DBG_Long(0, (MMP_ULONG)dev_dst+i);
                RTNA_DBG_Str(0, " = ");
                RTNA_DBG_Short(0, *(dev_src+i));
                RTNA_DBG_Str(0, ":");
                RTNA_DBG_Short(0, *(dev_dst+i));
                RTNA_DBG_Str(0, "\r\n");
                ret = 1;
                fail_cnt++;
                if (fail_cnt >= 0x20) {
                    break;
                }
            }
        }    
    }

    return  ret;
}
#endif

void Customer_Init(void)
{
	//PMUCtrl_Power_Gpio_En(MMP_TRUE);

#if (SNR_CLK_POWER_SAVING_SETTING != 0)
    MMPF_PLL_ChangeVIFClock(DPLL1, 1);      // CLK_GRP_SNR, 380MHz
    MMPF_PLL_WaitCount(50);
    MMPF_PLL_ChangeBayerClock(DPLL1, 1);    // CLK_GRP_BAYER, 380MHz
    MMPF_PLL_WaitCount(50);
    MMPF_PLL_ChangeISPClock(DPLL5, 1);      // CLK_GRP_COLOR, 480MHz
    MMPF_PLL_WaitCount(50);
#elif (MENU_MOVIE_SIZE_1080_60P_EN)
    MMPF_PLL_ChangeISPClock(DPLL2, 1);      // CLK_GRP_COLOR  528MHz
    MMPF_PLL_WaitCount(50);
#elif (CUSTOMIZED_DPLL1_CLOCK == 0)
    MMPF_PLL_ChangeVIFClock(DPLL1, 1);      // CLK_GRP_SNR, 380MHz
    MMPF_PLL_WaitCount(50);
#endif

#ifdef DEVICE_GPIO_SD_POWER
	// Turn-On SD Power
    if (DEVICE_GPIO_SD_POWER != MMP_GPIO_MAX) {
        MMPF_PIO_PadConfig(DEVICE_GPIO_SD_POWER, PAD_OUT_DRIVING(0), MMP_TRUE);
        MMPF_PIO_EnableOutputMode(DEVICE_GPIO_SD_POWER, MMP_TRUE, MMP_TRUE);
        MMPF_PIO_SetData(DEVICE_GPIO_SD_POWER, DEVICE_GPIO_SD_POWER_LEVEL, MMP_TRUE);
    }
#endif

	Customer_Init_CarDV();
}

void CE_JOB_DISPATCH_Task(void *p_arg)
{
    void (*FW_Entry)(void) = NULL;

#if (BOOT_FROM_SD)
	#if !defined(FAT_BOOT)
    extern stSDMMCHandler m_SDMMCArg[MMPF_SD_DEV_NUM];
    #endif
    #if (CHIP == MCR_V2)
    AIT_STORAGE_INDEX_TABLE2 *pIndexTable = (AIT_STORAGE_INDEX_TABLE2 *)AIT_BOOT_HEADER_ADDR;
    MMP_ULONG   ulTableAddr;
    #endif
    stSDMMCHandler  *pSDHandle;
#endif	// BOOT_FROM_SD

	#if (BOOT_FROM_SF == 1)
	AIT_STORAGE_INDEX_TABLE2 *pIndexTable = (AIT_STORAGE_INDEX_TABLE2 *)STORAGE_TEMP_BUFFER;
    MMP_ULONG   ulSifAddr = 0x0;
	MMP_USHORT  usCodeCrcValue = 0x0;
	MMP_ERR     err;
	#endif

    MMP_ULONG   ulDramFreq;

    RTNA_DBG_Str0("MBoot Job Task!\r\n");

    PMUCtrl_Power_Gpio_En(MMP_TRUE);
    MMPF_PLL_GetGroupFreq(CLK_GRP_DRAM, &ulDramFreq);
    ulDramFreq = ulDramFreq/1000;
    RTNA_DBG_PrintLong(0, ulDramFreq);

    /*DRAM Init*/
	#if (AUTO_DRAM_LOCKCORE)
    MMPF_DRAM_ScanNewLockCore(MMPS_System_GetConfig()->stackMemoryType,
                            &(MMPS_System_GetConfig()->ulStackMemorySize),
                            ulDramFreq,
                            MMPS_System_GetConfig()->stackMemoryMode);
	#endif

    /*Bandwidth Configuration*/
    #if (MCI_READ_QUEUE_4 == 1)
    MMPF_MCI_SetQueueDepth(4);
    #endif

    /*Platfrom Init*/
    //Customer_Init();		// CarDV
    //AIT_Init();			// CarDV

    #if defined(MBOOT_FW)
    /*Display Init*/
    RTNA_DBG_Str(0,"Display a moment\r\n");
    MMPC_Display_InitConfig(); 
    MMPC_Display_InitLCD();
    #endif

    /*Platfrom Init*/
    Customer_Init();

	MMPF_MMU_Deinitialize();

    #if (JTAG_EN)
    {
		AITPS_GBL pGBL = AITC_BASE_GBL; 
	    extern void RTNA_DBG_RxCallback(MMP_UBYTE size, volatile MMP_UBYTE *fifo);
		extern unsigned char RTNA_DBG_GetChar_NoWait(void);
		//int	cnt = JTAG_TIME_OUT * 10;//CZ patch...20160204

	    MMPF_Uart_EnableRx(DEBUG_UART_NUM, 1, (UartCallBackFunc *)&RTNA_DBG_RxCallback);
		
#if 1 //CZ patch...20160204
		RTNA_DBG_Str(0, "Press any key to enter JTAG before . running\r\n");
#else		
	    if (cnt != 0) {
			RTNA_DBG_Str(0, "Press any key to enter JTAG before . running\r\n");
	    }
		while (cnt != 0) {
			if (RTNA_DBG_GetChar_NoWait() != 0) {
				break;
			}
			RTNA_DBG_Str(0, ".");
			MMPF_OS_Sleep(100);
			cnt--;
		}
	    MMPF_Uart_DisableRx(DEBUG_UART_NUM);
		if (cnt || cnt == (JTAG_TIME_OUT * 10)) {
		    pGBL->GBL_BOOT_STRAP_CTL = MODIFY_BOOT_STRAP_EN |
		                        ARM_JTAG_MODE_EN | ROM_BOOT_MODE |
		                        JTAG_CHAIN_MODE_DIS | JTAG_CHAIN_CPUB_SET0;
		    pGBL->GBL_BOOT_STRAP_CTL &= ~(MODIFY_BOOT_STRAP_EN);
		    RTNA_DBG_Str(0,"Please use JTAG download ALL FW...\r\n");
		    while(1);
		}
#endif		
    }
    #endif

	/*Download firmware from moviNAND*/
	#if (BOOT_FROM_SD)
	/*moviNAND reset*/
	#if !defined(FAT_BOOT)
    pSDHandle = m_SDMMCArg+STOREFIRMWARE_SD_ID;
	MMPF_SD_InitHandler();
	MMPF_SD_InitialInterface(pSDHandle);
	MMPF_SD_SetTmpAddr(AIT_FW_TEMP_BUFFER_ADDR, 0);
	
	if(MMPF_SD_Reset(pSDHandle)) {
		MMPF_MMC_Reset(pSDHandle);
	}
	#endif
	
	RTNA_DBG_Str(0,"Start load moviNAND code\r\n");

	#if defined(FAT_BOOT)
	{
		#define		SECTOR_SIZE	(512)
		MMP_ERR		err;
		MMP_ULONG   ulfileHandle;
		MMP_ULONG   ulReadCount = 0;
		MMP_ULONG   FileSystemReadBuffer = AIT_BOOT_HEADER_ADDR;
		AIT_STORAGE_INDEX_TABLE2 *pIndex;
        MMP_ULONG ulLoop;
		extern void _SD_DevSetPad(MMP_ULONG ulPad);

            _SD_DevSetPad(SD_SOCKET0_MAP_ID);
            
            for(ulLoop = 0; ulLoop < FS_MAX_UNITS; ++ulLoop){
                MMPF_STORAGE_RegisterFSDevie(&gpFS_DeviceCustom[ulLoop], (MMP_ULONG)&acCacheMemory[ulLoop], FS_CACHE_LENGTH, 
                    (MMP_ULONG)(m_ubSDFreeClusterTable[ulLoop]), sizeof(*m_ubSDFreeClusterTable[ulLoop])); //Set custom FS device.  CarDV...                    
            }

            for(ulLoop = 0; ulLoop < MSDC_MAX_UNITS; ++ulLoop){    
                MMPF_STORAGE_RegisterMSDCDevie(&gpMSDC_DeviceCustom[ulLoop]);
            }  
        
		MMPS_System_AllocateFSBuffer(MMPS_SYSTEM_APMODE_DSC);
		err = MMPF_FS_FOpen("SD:\\SdFwCode.bin", "rb", &ulfileHandle);
		if (err == MMP_ERR_NONE)
		{						
			MMPF_FS_FRead(ulfileHandle, (void *)FileSystemReadBuffer, SECTOR_SIZE, &ulReadCount);	
			if (ulReadCount == SECTOR_SIZE) {
            	pIndex = (AIT_STORAGE_INDEX_TABLE2 *)FileSystemReadBuffer;
            	if((pIndex->ulHeader == INDEX_TABLE_HEADER) &&
                	(pIndex->ulTail == INDEX_TABLE_HEADER) &&
                	(pIndex->ulFlag & 0x1))
            	{ 
					MMPF_FS_FSeek(ulfileHandle,SECTOR_SIZE*(1+pIndex->ulTotalSectorsInLayer), MMP_FS_FILE_BEGIN);
					MMPF_FS_FRead(ulfileHandle, (void *)FileSystemReadBuffer, SECTOR_SIZE, &ulReadCount);
					
	                RTNA_DBG_PrintLong(0, pIndex->bt.ulStartBlock);
	                RTNA_DBG_PrintLong(0, pIndex->ulTotalSectorsInLayer);
        	        RTNA_DBG_PrintLong(0, pIndex->bt.ulDownloadDstAddress);
            	    
            		MMPF_FS_FRead(ulfileHandle, (void *)pIndex->bt.ulDownloadDstAddress, pIndex->bt.ulCodeSize, &ulReadCount);
            	}
            }
		}
	}
	#endif

	/*read boot area partition table*/
	MMPF_SD_ReadSector(pSDHandle, AIT_BOOT_HEADER_ADDR, 0, 1);
    //RTNA_DBG_PrintLong(0, pIndexTable->it[10].ulStartSec);
    RTNA_DBG_PrintLong(0, pIndexTable->bt.ulDownloadDstAddress);


    ulTableAddr = pIndexTable->ulTotalSectorsInLayer;
    MMPF_SD_ReadSector(pSDHandle, AIT_BOOT_HEADER_ADDR, ulTableAddr, 1);

    /*read boot code to  AIT_FW_START_ADDR (0x1000000 DRAM start address)*/
    if ((pIndexTable->ulHeader == INDEX_TABLE_HEADER) && 
		(pIndexTable->ulTail == INDEX_TABLE_TAIL) &&
		(pIndexTable->ulFlag & 0x1)) {
        MMPF_SD_ReadSector(pSDHandle, pIndexTable->bt.ulDownloadDstAddress, 
                            pIndexTable->bt.ulStartBlock,
                            (pIndexTable->bt.ulCodeSize + 511) >>
                            pIndexTable->ulAlignedBlockSizeShift);
        FW_Entry = (void (*)(void))(pIndexTable->bt.ulDownloadDstAddress);
        RTNA_DBG_PrintLong(0, pIndexTable->bt.ulDownloadDstAddress);
        RTNA_DBG_Str(0,"End load moviNAND code\r\n");
    }
    else {
		RTNA_DBG_PrintLong(0, pIndexTable->ulHeader);
		RTNA_DBG_PrintLong(0, pIndexTable->ulTail);
		RTNA_DBG_PrintLong(0, pIndexTable->ulFlag);
		RTNA_DBG_Str(0, "Invalid Index Table!!\r\n");
		while(1);
	}
	#endif //(BOOT_FROM_SD)

	#if (BOOT_FROM_NAND)&&(USING_SM_IF)
    RTNA_DBG_Str(0,"Start load NAND code\r\n");
    /*switch SD to access boot area*/
    //MMPF_MMC_SwitchBootPartition(STOREFIRMWARE_SD_ID, 1);			
    /*read boot area partition table*/
    MMPF_NAND_SetMemory(AIT_FW_TEMP_BUFFER_ADDR);
    MMPF_NAND_InitialInterface();
    MMPF_NAND_Reset();
    MMPF_NAND_ReadSector(AIT_BOOT_HEADER_ADDR, 0,1);

    /*read boot code to  AIT_FW_START_ADDR (0x1000000 DRAM start address)*/
    MMPF_NAND_ReadSector(AIT_FW_START_ADDR, pAit_boot_header->it[1].ulStartSec, pAit_boot_header->it[1].ulSecSize);
    RTNA_DBG_Str(0,"End load NAND code\r\n");	
    #endif //(BOOT_FROM_NAND)

	#if (BOOT_FROM_SF)
	SF_Module_Init();
	MMPF_SF_SetIdBufAddr((MMP_ULONG)(STORAGE_TEMP_BUFFER - 0x1000));
	MMPF_SF_SetTmpAddr((MMP_ULONG)(STORAGE_TEMP_BUFFER - 0x1000));
  	MMPF_SF_InitialInterface(MMPF_SIF_PAD_MAX);
	
	err = MMPF_SF_Reset();
	if (err) {
		RTNA_DBG_Str(0, "SIF Reset error !!\r\n");
		return;
	}
	
	MMPF_SF_FastReadData(ulSifAddr, STORAGE_TEMP_BUFFER, 512);  //AitMiniBootloader Header Table
	ulSifAddr = ulSifAddr + (0x1 << 12)*(pIndexTable->ulTotalSectorsInLayer);
	MMPF_SF_ReadData(ulSifAddr, (STORAGE_TEMP_BUFFER), 512);  //AitBootloader Header Table
	ulSifAddr = ulSifAddr + (0x1 << 12)*(pIndexTable->ulTotalSectorsInLayer);
	#if 0//CZ patch...20160204
	while(1) {
		MMPF_SF_ReadData(ulSifAddr, (STORAGE_TEMP_BUFFER), 512);  //Firmware Header Table
		if ((pIndexTable->ulHeader == INDEX_TABLE_HEADER) && 
			(pIndexTable->ulTail == INDEX_TABLE_TAIL) &&
			(pIndexTable->ulFlag & 0x1)) {
				RTNA_DBG_Str(0, "PASS\r\n");
			}
	    else {
	    		RTNA_DBG_Str(0, "FAIL\r\n");
	    }
	}
	#else
	MMPF_SF_ReadData(ulSifAddr, (STORAGE_TEMP_BUFFER), 512);  //Firmware Header Table
	#endif
	if ((pIndexTable->ulHeader == INDEX_TABLE_HEADER) && 
		(pIndexTable->ulTail == INDEX_TABLE_TAIL) &&
		(pIndexTable->ulFlag & 0x1)) {
			RTNA_DBG_PrintLong(0, pIndexTable->bt.ulDownloadDstAddress);
			FW_Entry = (void (*)(void))(pIndexTable->bt.ulDownloadDstAddress);

			#if (CHIP == MCR_V2)
			if(pIndexTable->ulFlag & 0x40000000) { //check bit 30
				MMPF_SF_EnableCrcCheck(MMP_TRUE);
			}
			#endif 
			RTNA_DBG_Str(0, "SIF downlod start\r\n");
			
			RTNA_DBG_PrintLong(0, pIndexTable->bt.ulStartBlock);

			MMPF_SF_FastReadData(pIndexTable->bt.ulStartBlock << pIndexTable->ulAlignedBlockSizeShift, 
									pIndexTable->bt.ulDownloadDstAddress, 
									pIndexTable->bt.ulCodeSize);

			#if (CHIP == MCR_V2)
			if(pIndexTable->ulFlag & 0x40000000) { //check bit 30
				usCodeCrcValue = MMPF_SF_GetCrc();
				MMPF_SF_FastReadData(ulSifAddr + 0x1000, STORAGE_TEMP_BUFFER, 2); //Read CRC block value
				if(*((MMP_USHORT*)STORAGE_TEMP_BUFFER) == usCodeCrcValue) {
					RTNA_DBG_Str(0, "CRC pass \r\n");
				}
				else {
					RTNA_DBG_Str(0, "CRC check fail !!! \r\n");
					RTNA_DBG_PrintShort(0, usCodeCrcValue);
					RTNA_DBG_PrintShort(0, *((MMP_USHORT*)STORAGE_TEMP_BUFFER));
					while(1);
				}
			}	
			#endif
			RTNA_DBG_Str(0, "SIF downlod End\r\n");
			RTNA_DBG_Str(0, "\r\n");
			{//CZ patch...20160204
				AITPS_GBL pGBL = AITC_BASE_GBL; 
			    extern void RTNA_DBG_RxCallback(MMP_UBYTE size, volatile MMP_UBYTE *fifo);
				extern unsigned char RTNA_DBG_GetChar_NoWait(void);

				if (RTNA_DBG_GetChar_NoWait() != 0) {
				    MMPF_Uart_DisableRx(DEBUG_UART_NUM);
					pGBL->GBL_BOOT_STRAP_CTL = MODIFY_BOOT_STRAP_EN |
				                        ARM_JTAG_MODE_EN | ROM_BOOT_MODE |
				                        JTAG_CHAIN_MODE_DIS | JTAG_CHAIN_CPUB_SET0;
				    pGBL->GBL_BOOT_STRAP_CTL &= ~(MODIFY_BOOT_STRAP_EN);
				    RTNA_DBG_Str(0,"Please use JTAG download ALL FW...\r\n");	
				    while(1);

				}
    		}

            //Test only
            MMPF_MMU_Initialize((MMP_ULONG *)MMU_TRANSLATION_TABLE_ADDR,(MMP_ULONG *)MMU_COARSEPAGE_TABLE_ADDR);
            RTNA_DBG_Str(0, "\r\nGoto ALL_FW...\r\n");

    		FW_Entry();	//enter the firmware entry
    		while (1);
	}
	else {
		RTNA_DBG_PrintLong(0, pIndexTable->ulHeader);
		RTNA_DBG_PrintLong(0, pIndexTable->ulTail);
		RTNA_DBG_PrintLong(0, pIndexTable->ulFlag);
		RTNA_DBG_Str(0, "Invalid Index Table!!\r\n");
		while(1);
	}
	#endif //(BOOT_FROM_SF)
	
    /* Jump PC to AIT_FW_START_ADDR */
    FW_Entry = (void (*)(void))(AIT_FW_START_ADDR);
    FW_Entry();

    while (1) ;
}


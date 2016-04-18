//==============================================================================
//
//  File        : bootloader.h
//  Description : 
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

#include "customer_config.h"

#include "includes_fw.h"
#include "mmpf_typedef.h"

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
#if (BOOT_FROM_NAND)
#include "mmpf_nand.h"
#endif

//==============================================================================
//
//                              CONSTANT
//
//==============================================================================
// 0x01000000 ~ 0x02FFFFFF : 32MB
// 0x01000000 ~ 0x04FFFFFF : 64MB
#define AIT_FW_TEMP_BUFFER_ADDR  	(0x00106000)
#define AIT_BOOT_HEADER_ADDR     	(0x00106200)
#define AIT_FW_START_ADDR        	(0x01000000)
#define AIT_SYS_MEDIA_START_ADDR    (AHC_GUI_TEMP_BASE_ADDR) // reference AHC_GUI_TEMP_BASE_ADDR 
#define AIT_SYS_IMAGE_TEMP_ADDR	    (AIT_SYS_MEDIA_START_ADDR + 0x00280000)
#define AIT_SYS_IMAGE_TEMP_ADDR1    (AIT_SYS_IMAGE_TEMP_ADDR +  0x00080000)

#define STORAGE_TEMP_BUFFER 		(AIT_FW_TEMP_BUFFER_ADDR)
#define AIT_SF_WORK_BUFFER_ADDR  	(AIT_FW_TEMP_BUFFER_ADDR)
#define MBR_ADDRESS					(0)

#define STOREFIRMWARE_SD_ID     	(0)

//////////////////////////////////////////////////////////////////////////////////
#define TV_NTSC			0
#define TV_PAL			1

#define LCD_NORMAL		0
#define LCD_REVERSE		1

#define LCD_ROTATE_ON	0
#define LCD_ROTATE_OFF	1

#define HDMI_1080I		0
#define HDMI_720P		1
#define HDMI_480P		2

#define	DISP_NONE		0
#define	DISP_LCD		1
#define	DISP_LCDV		2
#define	DISP_NTSC		3
#define	DISP_PAL		4
#define	DISP_HDMI		5
#define	DISP_CCIR		6//for test ccir656

#define	LOWB_WIDTH						(48)
#define	LOWB_HEIGHT						(48)
#define	POWER_ON_WIDTH					(320)
#define	POWER_ON_HEIGHT					(240)
#define	SPLASH_COLOR_DEPTH				(16)

#define	POWER_ON_IDX					(0)
#define	BATTERY_LOW_IDX					(1)

#define BATTERY_LOW_BOOT_WARNING_TIME	(2000)

// Partition 10 for SF:0	// Code Resource
// Partition 11 for SF:1	// Factory Resource
// Partition 12 for SF:2	// User Resource
// ...
#define	SPLASH_PARTITION_START			(13)

typedef struct {
	unsigned short disp_w;
	unsigned short disp_h;
	unsigned short disp_clrs; // In ALL_FW, it would be the default off screen buffers to be created.
	                          // It should be 24/32 or 16. 24 is now treated as 32.
	unsigned short disp_type; // Currently this is for power-on image only.
} DISP_PROP;
//////////////////////////////////////////////////////////////////////////////////

//==============================================================================
//
//  File        : sf_cfg.c
//  Description : SPI Nor Flash configure file
//  Author      : Rony Yeh
//  Revision    : 1.0
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================
#include "config_fw.h" // add for use (USING_SF_IF )

#if (USING_SF_IF)
#include "sf_cfg.h" // add for use (module define )
#include "mmpf_sf.h" 

//==============================================================================
//
//                              EXTERNAL FUNC
//
//==============================================================================



//==============================================================================
//
//                              SPI NOR Flash Parameter assignment
//
//==============================================================================

SPINorProfile SPINORConfigMatrix[SPIMAXMODULENO] = {

#if (SPI_NorFlash_DefaultSetting) //The array member must be leave for Default Setting
{   0xFFFFFFFF,    // DeviceID   SPI_NorFlash_DefaultSetting
    0x400000,	   // TotalSize	256Mb 32MB
	66000,		   //parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor//CZ patch...20160204
	0x1000,		   // 4K for each Sector	
	0x8000,		   // 32K for each Block
	
	1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_WINBOND_25Q32)
{   0x00EF4016,    // DeviceID   WINBOND_25Q32
    0x400000,     // TotalSize - 4MB CarDV
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block
    
	1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	3,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x02,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_WINBOND_25Q64)
{   0x00EF4017,    // DeviceID   WINBOND_25Q64
    0x800000,     // TotalSize - 8MB CarDV
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor//CZ patch...20160204
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	3,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x02,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_WINBOND_25Q128)
{   0x00EF4018,    // DeviceID   WINBOND_25Q128
    0x1000000,     // TotalSize - 16MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	3,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x02,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_WINBOND_25Q256FV)
{   0x00EF4019,    // DeviceID   WINBOND_25Q256FV
    0x2000000,     // TotalSize - 32MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block    

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	3,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x02,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_WINBOND_25X10)
{   0x0084B5C3,    // DeviceID   WINBOND_25X10
    0x200000,      // TotalSize
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block
    
    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	3,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x02,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_MXIC_25L512)
{   0x009ACF4A,    // DeviceID   MXIC_25L512
    0x100000,      // TotalSize
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block
    1,             // Capability to support Quad SPI read
    1,             // Capability to support Dual SPI read
    0,             // Capability to support Quad SPI write
    0,             // option to seepdup write volatile SR process
    0,             // 4 bytes address parameter
    0,             // Support AAI-Write parameter
},
#endif   // #if (SPI_NorFlash_WINBOND_25Q128)

#if (SPI_NorFlash_MXIC_25L12835F)
{   0x00C22018,    // DeviceID   MXIC_25L12835F
	0x1000000,	   // TotalSize	
	66000,		   //parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
	0x1000,		   // 4K for each Sector	
	0x8000,		   // 32K for each Block	

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	3,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x20,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    0x13,                // Command for read data <4 Byte Address>
    0x0C,                // Command for fast read data <4 Byte Address>
    0x3C,                // Command for Dual fast read data <4 Byte Address>
    0x12,                // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	0x21,	             // Command for Erase Sector <4 Byte Address>
},
#endif   // #if (SPI_NorFlash_MXIC_25L12835F)

#if (SPI_NorFlash_GD_25Q64B)
{   0x00C84017,    // DeviceID   GD_25Q64B
    0x800000,      // TotalSize
    66000,         //parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor//CZ patch...20160204
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block    

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif   // #if (SPI_NorFlash_WINBOND_25Q128)

#if (SPI_NorFlash_GD_25Q128B)
{   0x00C84018,    // DeviceID
    0x1000000,     // TotalSize
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_KH25L3233F)
{   0xC22016,      // DeviceID, conflict with SPI_NorFlash_MXIC_25L3206
    0x00400000,    // TotalSize - 4MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_MXIC_25L3206)
{   0xC22016,      // DeviceID, conflict with SPI_NorFlash_KH25L3233F
    0x00400000,    // TotalSize - 4MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x10000,       // 64K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_MXIC_25L6406E)
{   0xC22017,      // DeviceID
    0x00800000,    // TotalSize - 8MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x10000,       // 64K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_ESMT_64L)
{   0x8C4117,      // DeviceID
    0x00800000,    // TotalSize - 8MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_EN25Q32A)
{   0x1C3016,      // DeviceID
    0x00400000,    // TotalSize - 4MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

#if (SPI_NorFlash_EN25Q128A)
{   0x1C7018,      // DeviceID
    0x01000000,    // TotalSize - 16MB
    66000,         // parameter to create <ulSifClkDiv> EX: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor
    0x1000,        // 4K for each Sector
    0x8000,        // 32K for each Block

    1,             // Capability to support Dual SPI read
	0,             // option to seepdup write volatile SR process
	0,             // 4 bytes address mode enable/disable
	0,             // Support AAI-Write prrameter 	
	0,             // Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
	0,             // Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP
	2,			   // Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>	
	0x10,		   // Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
	
    READ_DATA,           // Command for read data <3 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <3 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <3 Byte Address>
    PAGE_PROGRAM,        // Command for write data <3 Byte Address>
    READ_DATA,           // Command for read data <4 Byte Address>
    FAST_READ_DATA,      // Command for fast read data <4 Byte Address>
    DUAL_FAST_READ_DATA, // Command for Dual fast read data <4 Byte Address>
    PAGE_PROGRAM,        // Command for write data <4 Byte Address>
	SECTOR_ERASE,        // Command for Erase Sector <3 Byte Address>
	SECTOR_ERASE,	     // Command for Erase Sector <4 Byte Address>
},
#endif

};

int SF_Module_Init(void)
{
    MMP_ULONG dev;
            
    for(dev = 0; dev < SPIMAXMODULENO; dev++) {
        MMPF_SF_Register( (MMP_UBYTE)dev, &SPINORConfigMatrix[dev] );
    }
	
    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6" 
#pragma O0
ait_module_init(SF_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif   // #if (USING_SF_IF)


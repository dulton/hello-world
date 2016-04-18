//==============================================================================
//
//  File        : gps.c
//  Description : Firmware Sensor Control File
//  Author      : Philip Lin
//  Revision    : 1.0
//
//=============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "customer_config.h"

#include "Customer_Config.h"
#include "GPS_ctl.h"
#include "AIT_utility.h"
#include "mmpf_uart.h"
#if (GPS_CONNECT_ENABLE && (GPS_MODULE == GPS_MODULE_NMEA0183))
#include "GPS_nmea0183_ANA.h"

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

// new added commands for u-blox 7
//******************************************************************************
//                              [GPS ANA Command]
//******************************************************************************
/*
 * 1-1. Enable ANA function (AssistNow Autonomous), 3D fix
 * B5 62 06 23 28 00 00 00 4C 66 00 00 00 00 00 00 03 10 00 00 01 00 00 00 00 00 00 00 00 00 00 00 01 01 00 00 00 00 00 00 00 00 00 00 00 00 19 DE
 */
static MMP_UBYTE 	ubANA3DFix[48] 			= {0xB5, 0x62, 0x06, 0x23, 0x28, 0x00, 0x00, 0x00, 0x4C, 0x66, 
											   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x10, 0x00, 0x00, 
											   0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
											   0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
											   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0xDE}; // 48

/*
 * 1-2. Automotive mode, PDOP Mask = 35, Ele = 3, Static hold 1.0m/s
 * B5 62 06 24 24 00 FF FF 04 03 00 00 00 00 10 27 00 00 03 00 5E 01 FA 00 64 00 2C 01 64 3C 00 00 00 00 00 00 00 00 00 00 00 00 17 99
 */
static MMP_UBYTE 	ubAutomotiveMode[44] 	= {0xB5, 0x62, 0x06, 0x24, 0x24, 0x00, 0xFF, 0xFF, 0x04, 0x03, 
											   0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00, 0x03, 0x00, 
											   0x5E, 0x01, 0xFA, 0x00, 0x64, 0x00, 0x2C, 0x01, 0x64, 0x3C, 
											   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
											   0x00, 0x00, 0x17, 0x99}; // 44

/*
 * 1-3. NMEA configuration: Filters(12346)&1-Extended(3-digit), Max SVs(16), Compatibility mode(V), GP
 * B5 62 06 17 0C 00 2F 23 10 03 00 00 00 00 01 01 00 00 90 D8
 */
static MMP_UBYTE 	ubNMEAconfig[20] 		= {0xB5, 0x62, 0x06, 0x17, 0x0C, 0x00, 0x2F, 0x23, 0x10, 0x03, 
											   0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x90, 0xD8}; // 20

/*
 * 1-4. Enable PRN128
 * B5 62 06 16 08 00 01 01 03 00 D1 A3 06 00 A3 E4
 */
static MMP_UBYTE 	ubPRN128[16] 			= {0xB5, 0x62, 0x06, 0x16, 0x08, 0x00, 0x01, 0x01, 0x03, 0x00, 
											   0xD1, 0xA3, 0x06, 0x00, 0xA3, 0xE4}; // 16

/*
 * 1-5. del GLL (to save space that baud rate = 9600) (UART 1)
 * B5 62 06 01 08 00 F0 01 01 00 01 01 01 01 05 3A
 */
static MMP_UBYTE 	ubGLLdel[16] 			= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x01, 0x00, 
											   0x01, 0x01, 0x01, 0x01, 0x05, 0x3A}; // 16

/*
 * 1-6. del VTG (to save space that baud rate = 9600) (UART 1)
 * B5 62 06 01 08 00 F0 05 01 00 01 01 01 01 09 56
 */
static MMP_UBYTE 	ubVTGdel[16] 			= {0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x01, 0x00, 
											   0x01, 0x01, 0x01, 0x01, 0x09, 0x56}; // 16

/*
 * 1-7. Save current setting Command: 
 * B5 62 06 09 0D 00 00 00 00 00 FF FF 00 00 00 00 00 00 03 1D AB
 */
static MMP_UBYTE 	ubSettingSave[21] 		= {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 
											   0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x1D, 
											   0xAB}; // 21
//******************************************************************************											   
//                              [GPS Common Command]
//******************************************************************************

static MMP_UBYTE 	ubHotStart[12] 			= {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 
										   	   0x10, 0x68}; // 12

static MMP_UBYTE 	ubWarmStart_1[12] 		= {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0x01, 0x00, 0x02, 0x00, 
										   	   0x11, 0x6C}; // 12

static MMP_UBYTE 	ubWarmStart_2[12] 		= {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0x01, 0x80, 0x02, 0x00, 
										   	   0x91, 0xEC}; // 12

static MMP_UBYTE 	ubColdStart[12] 		= {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0xFF, 0x87, 0x02, 0x00, 
										   	   0x96, 0xF9}; // 12

static MMP_UBYTE 	ubBR57600[28] 			= {0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 
											   0xD0, 0x08, 0x00, 0x00, 0x00, 0xE1, 0x00, 0x00, 0x07, 0x00, 
										   	   0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0xC9}; // 28
//******************************************************************************
//                              [GPS User Command]
//******************************************************************************
/*
 * Switch to Glonass Command:
 * B5 62 06 3E 24 00 00 00 16 04 00 04 FF 00 00 00 00 01 01 01 03 00 00 00 00 01 05 00 03 00 00 00 00 01 06 08 FF 00 01 00 00 01 A4 0D
 */
static MMP_UBYTE 	ubGLONASSswitch[44] 	= {0xB5, 0x62, 0x06, 0x3E, 0x24, 0x00, 0x00, 0x00, 0x16, 0x04, 
											   0x00, 0x04, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 
											   0x03, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x00, 0x03, 0x00, 
											   0x00, 0x00, 0x00, 0x01, 0x06, 0x08, 0xFF, 0x00, 0x01, 0x00, 
											   0x00, 0x01, 0xA4, 0x0D}; // 44

/*
 * Switch to GPS Command:
 * B5 62 06 3E 24 00 00 00 16 04 00 04 FF 00 01 00 00 01 01 01 03 00 01 00 00 01 05 00 03 00 01 00 00 01 06 08 FF 00 00 00 00 01 A6 45
 */
static MMP_UBYTE 	ubGPSswitch[44] 		= {0xB5, 0x62, 0x06, 0x3E, 0x24, 0x00, 0x00, 0x00, 0x16, 0x04, 
											   0x00, 0x04, 0xFF, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01, 0x01, 
											   0x03, 0x00, 0x01, 0x00, 0x00, 0x01, 0x05, 0x00, 0x03, 0x00, 
											   0x01, 0x00, 0x00, 0x01, 0x06, 0x08, 0xFF, 0x00, 0x00, 0x00, 
											   0x00, 0x01, 0xA6, 0x45}; // 44
											   
//******************************************************************************
// End of new added commands for u-blox 7											   

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================
//extern struct _3RD_PARTY_GPS  GPS_Module_Obj;
struct _3RD_PARTY_GPS  GPS_Module_Obj;

//------------------------------------------------------------------------------
//  Function    : GPS_SetCommand_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_SetCommand_ByModule(struct _3RD_PARTY_GPS *pthis, void *pCommAry)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    sRet = GPS_SetCommand(pCommAry);
    return sRet;
}

//------------------------------------------------------------------------------
//  Function    : GPS_Initiate_ANA_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_Initiate_ANA_ByModule(struct _3RD_PARTY_GPS *pthis)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    #if 1 // new added commands for u-blox 7
    printc(FG_YELLOW("Run GPS_Initiate_ANA\r\n")); 
    sRet = pthis->GPS_SetCommand(pthis, (void *)&ubANA3DFix); //GPS_SetCommand_ByModule
    MMPF_OS_Sleep(20);
    
    sRet |= pthis->GPS_SetCommand(pthis, (void *)&ubAutomotiveMode);    
    MMPF_OS_Sleep(20);

    sRet |= pthis->GPS_SetCommand(pthis, (void *)&ubNMEAconfig);    
    MMPF_OS_Sleep(20);
    
    sRet |= pthis->GPS_SetCommand(pthis, (void *)&ubPRN128);    
    MMPF_OS_Sleep(20);
    
    sRet |= pthis->GPS_SetCommand(pthis, (void *)&ubGLLdel);    
    MMPF_OS_Sleep(20);
    
    sRet |= pthis->GPS_SetCommand(pthis, (void *)&ubVTGdel);    
    MMPF_OS_Sleep(20);
    
    sRet |= pthis->GPS_SetCommand(pthis, (void *)&ubSettingSave);
    MMPF_OS_Sleep(20);
    
    #else
    sRet = pthis->GPS_SetCommand(pthis, (void *)&NMEACMD_CFG_SET_EXPERT);   
    MMPF_OS_Sleep(10);
    
    sRet |= pthis->GPS_SetCommand(pthis, (void *)&NMEACMD_CFG_SL_CFG);    
    #endif

    return sRet;
}

//------------------------------------------------------------------------------
//  Function    : GPS_SetHotStart_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_SetHotStart_ByModule(struct _3RD_PARTY_GPS *pthis)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc(FG_YELLOW("Run GPS_SetHotStart\r\n")); 
    sRet = pthis->GPS_SetCommand(pthis, (void *)&ubHotStart); //GPS_SetCommand_ByModule

    return sRet;
}

//------------------------------------------------------------------------------
//  Function    : GPS_SetWarmStart_1_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_SetWarmStart_1_ByModule(struct _3RD_PARTY_GPS *pthis)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc(FG_YELLOW("Run GPS_SetWarmStart_1\r\n")); 
    sRet = pthis->GPS_SetCommand(pthis, (void *)&ubWarmStart_1); //GPS_SetCommand_ByModule

    return sRet;
}
//------------------------------------------------------------------------------
//  Function    : GPS_SetWarmStart_2_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_SetWarmStart_2_ByModule(struct _3RD_PARTY_GPS *pthis)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc(FG_YELLOW("Run GPS_SetWarmStart_2\r\n")); 
    sRet = pthis->GPS_SetCommand(pthis, (void *)&ubWarmStart_2); //GPS_SetCommand_ByModule

    return sRet;
}
//------------------------------------------------------------------------------
//  Function    : GPS_SetColdStart_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_SetColdStart_ByModule(struct _3RD_PARTY_GPS *pthis)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc(FG_YELLOW("Run GPS_SetColdStart\r\n")); 
    sRet = pthis->GPS_SetCommand(pthis, (void *)&ubColdStart); //GPS_SetCommand_ByModule

    return sRet;
}

//------------------------------------------------------------------------------
//  Function    : GPS_SetBR57600_ByModule
//  Description :
//------------------------------------------------------------------------------

MMP_ERR GPS_SetBR57600_ByModule(struct _3RD_PARTY_GPS *pthis)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    
    printc(FG_YELLOW("Run GPS_SetBR57600\r\n")); 
    sRet = pthis->GPS_SetCommand(pthis, (void *)&ubBR57600); //GPS_SetCommand_ByModule

    return sRet;
}
//------------------------------------------------------------------------------
//  Function    : GPS_Module_Init
//  Description :
//------------------------------------------------------------------------------

int GPS_Module_Init(void)
{
    MMPF_3RDParty_GPS_Register(&GPS_Module_Obj);
    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6", zidata = "initcall6"
#pragma O0
ait_module_init(GPS_Module_Init);
#pragma
#pragma arm section rodata, rwdata, zidata

#endif // (GPS_CONNECT_ENABLE && (GPS_MODULE == GPS_MODULE_NMEA0183))
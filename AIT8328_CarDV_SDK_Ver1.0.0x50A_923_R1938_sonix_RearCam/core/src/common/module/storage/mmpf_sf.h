#ifndef _MMPF_SF_H_
#define _MMPF_SF_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define SPINORFLASHMAXNUM       0x20 // Maximum member numbers of array SPINORindex[SPINORFLASHMaxNUM]    
#define ERRCHECK_RETURN(bERR)       if(bERR) {return bERR;}
#define SIF_DBG_LEVEL               (0) 
//------------------------------------------------------------------------------
// SF Command Index
//------------------------------------------------------------------------------
#define WRITE_STATUS1           0x01 // for write status register1
#define WRITE_STATUS2           0x31 // for write status register2
#define WRITE_STATUS3           0x11 // for write status register3
#define PAGE_PROGRAM            0x02
#define QUAD_PAGE_PROGRAM       0x32
#define READ_DATA               0x03
#define WRITE_DISABLE           0x04
#define EXIT_OTP_MODE           0x04
#define READ_STATUS1            0x05 // for read status register1
#define READ_STATUS2            0x35 // for read status register2
#define READ_STATUS3            0x15 // for read status register3
#define WRITE_ENABLE            0x06
#define FAST_READ_DATA          0x0B
#define DUAL_FAST_READ_DATA     0x3B
#define WRITE_ENABLEFORVOLATILE 0x50
#define QUAD_FAST_READ_DATA     0x6B
#define SECTOR_ERASE            0x20
#define ENTER_OTP_MODE          0x3A
#define READ_UNIQUE_ID			0x4B 
#define BLOCK_ERASE             0x52
#define CHIP_ERASE              0x60
#define EBSY                    0x70
#define DBSY                    0x80
#define READ_DEVICE_ID          0x9F
#define RELEASE_DEEP_POWER_DOWN 0xAB
#define ADDR_AUTO_INC_WRITE     0xAD
#define DEEP_POWER_DOWN         0xB9
#define RESETCOMMAND			0x99
#define ENABLERESETCOMMAND		0x66
#define CLEARSR        			0x30
#define EN4B					0xB7
#define EX4B					0xE9

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SF Device ID
//------------------------------------------------------------------------------
#define EON_EN25F10             0x1C3111
#define EON_EN25F20             0x1C3112
#define EON_EN25F40             0x1C3113
#define EON_EN25F80             0x1C3114
#define EON_EN25Q16A            0x1C3015
#define EON_EN25Q32A            0x1C3016
#define EON_EN25QH128A          0x1C7018

#define WINBOND_25X10           0xEF3011
#define WINBOND_25X20           0xEF3012
#define WINBOND_25X40           0xEF3013
#define WINBOND_25X80           0xEF3014
#define WINBOND_25Q80           0xEF4014
#define WINBOND_25Q16           0xEF4015
#define WINBOND_25Q32           0xEF4016
#define WINBOND_25Q64           0xEF4017
#define WINBOND_25Q128			0xEF4018
#define WINBOND_25Q256FV		0xEF6019

#define MXIC_25L512             0xC22010
#define MXIC_25L1005            0xC22011
#define MXIC_25L2005            0xC22012
#define MXIC_25L4005            0xC22013
#define MXIC_25L8005            0xC22014
//#define MXIC_25L3206            0xC22016		// CarDV, not suppoprt, same ID as KH25L3233F, but block size is 64KB.
#define KH25L3233F              0xC22016
#define MXIC_25L6406E           0xC22017
#define MXIC_25L12835F			0xC22018

#define GD_25Q32B               0xC84016
#define GD_25Q64B               0xC84017
#define GD_25Q128B              0xC84018

#define SST_25VF080B            0xBF258E

#define ESMT_25L16              0x8C2015
#define ESMT_25L16PA            0x8C2115
#define ESMT_64L                0x8C4117

/* Spanson Flash */
#define S25FL004A               0x00010212
#define S25FL016A               0x00010214
#define S25FL032A               0x00010215
#define S25FL064A               0x00010216  /*supposed support*/
#define S25FL128P               0x00012018  /*only S25FL128P0XMFI001, Uniform 64KB secotr*/
                                            /*not support S25FL128P0XMFI011, Uniform 256KB secotr*/
                                            /*because #define SPI_BLOCK_SIZE 65536 */

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SF Sector Size
//------------------------------------------------------------------------------
#define SerialFlashSectorSize   0x1000
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SF Status Register
//------------------------------------------------------------------------------
#define WRITE_IN_PROGRESS       0x01
#define WRITE_ENABLE_LATCH      0x02
//------------------------------------------------------------------------------

// SF Psd Set
//------------------------------------------------------------------------------
#define SET_PAD0                0x2000
#define SET_PAD1                0x2100
#define SET_PAD2                0x2200
#define RESET_BOOT_DEVICE_AND_SIF_PAD   0xE300
//------------------------------------------------------------------------------

//==============================================================================
//
//                              ENUMERATIONS
//
//==============================================================================

typedef enum _MMPF_SIF_PAD
{
    MMPF_SIF_PAD_0 = 0, //pad set PBGPIO0~5
    MMPF_SIF_PAD_MAX
} MMPF_SIF_PAD;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPF_SIF_INFO {
    MMP_ULONG ulDevId;          // Device ID
    MMP_ULONG ulSFSectorSize;   // Size per sector in unit of byte
    MMP_ULONG ulSFBlockSize;    // Size per Block in unit of byte
    MMP_ULONG ulSFTotalSize;    // Total size in unit of byte
} MMPF_SIF_INFO;

typedef  struct _SPINorProfile {
    MMP_ULONG   DeviceID;           //Device ID , check spec for it	
    MMP_ULONG   TotalSize;			//size of Bytes for total module	
    MMP_ULONG   SIFCLKDividor;      //parameter to create <ulSifClkDiv> Example: ulSifClkDiv = (ulG0Freq+32999)/SIFCLKDividor;
	MMP_ULONG   SectorSIZE;			//size of Bytes for each Sectpr 
	MMP_ULONG   BlockSIZE;			//size of Bytes for each Block	

	//MMP_UBYTE   ReadQuadSPI;  	//Capability to support Quad SPI when reading,check SPEC <could not active if Dual-SPI read is active>
    MMP_UBYTE   ReadDualSPI;  		//Capability to support Dual SPI when reading,check SPEC <could not active if Quad-SPI read is active>
	//MMP_UBYTE   WriteQuadSPI;     //Capability to support Quad SPI when writing,check SPEC
	MMP_UBYTE   EnableVolatileWrite;//cmd<50H>,speedup the write Volatile SR process
    MMP_UBYTE   FourByteAddress;    //Enable 4 bytes address type, (1)4 bytes , (0)3 bytes, MercuryV2 MP only,if not MercuryV2, the value is dont'c care
    MMP_UBYTE   SupportAAIWrite; 	//Capaility to support AAI-Write <SST>
    MMP_UBYTE   SupportClearSR; 	//Clear Status Register<Command 30h> in the reset API;(0)Not ClearSR (1)ClearSR,make sure module support capability before assigning
    
	MMP_UBYTE   ADPMode;            //Execute/NotExecute ADP mode setting,some module don't support this function<ex:MXIC>;(0)Not Execute ADP , (1)Execute ADP	
    MMP_UBYTE   SR4ByteAdd;         //Status Register Group Index to Enable/Disable <ADP function> for 4 Byte Address mode, group1<0x05> group2<0x35> group3<0x15>
    MMP_UBYTE   SRBit4ByteAdd;      //Status Register Bit Number to Enable/Disable <ADP function> for 4 Byte Address mode,EX: bit4=0x10,bit5=0x20
    //MMP_UBYTE   SRQuadEnable;       //Status Register Group Index to Enable Quad SPI, group1<0x05> group2<0x35> group3<0x15>
    //MMP_UBYTE   SRBitQuadEnable;    //Status Register Bit Number to Enable Quad SPI,EX: bit4=0x10,bit5=0x20
	   
    MMP_UBYTE   ReadDataCMD3Byte;     //Command for read data <3 Byte Address>
    MMP_UBYTE   FastReadDataCMD3Byte; //Command for fast read data <3 Byte Address>
    MMP_UBYTE   DualReadDataCMD3Byte; //Command for Dual fast read data <3 Byte Address>
    //MMP_UBYTE   QuadReadDataCMD3Byte; //Command for Quad fast read data <3 Byte Address>
    MMP_UBYTE   WriteDataCMD3Byte;    //Command for write data <3 Byte Address>
    //MMP_UBYTE   QuadWriteDataCMD3Byte;//Command for Quad write data <3 Byte Address>
    MMP_UBYTE   ReadDataCMD4Byte;     //Command for read data <4 Byte Address>
    MMP_UBYTE   FastReadDataCMD4Byte; //Command for fast read data <4 Byte Address>
    MMP_UBYTE   DualReadDataCMD4Byte; //Command for Dual fast read data <4 Byte Address>
    //MMP_UBYTE   QuadReadDataCMD4Byte; //Command for Quad fast read data <4 Byte Address>
    MMP_UBYTE   WriteDataCMD4Byte;    //Command for write data <4 Byte Address>
    //MMP_UBYTE   QuadWriteDataCMD4Byte;//Command for Quad write data <4 Byte Address>

	MMP_UBYTE   EraseSectorCMD3Byte;  //Command for Erase Sector <3 Byte Address>
	MMP_UBYTE   EraseSectorCMD4Byte;  //Command for Erase Sector <4 Byte Address>
} SPINorProfile;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_SF_Register(MMP_UBYTE ubSFIndex , SPINorProfile *pArrayAdd);
MMP_ERR MMPF_SF_ClearCommand(void);
MMP_ERR MMPF_SF_ResetCommand(void);
MMP_ERR MMPF_SF_ENEX4B(MMP_UBYTE ADPStatus);
MMP_ERR MMPF_SF_CheckReady(void);
MMP_ERR MMPF_SF_ReadStatus(MMP_UBYTE SRnum, MMP_UBYTE *ubStatus);
MMP_ERR MMPF_SF_WriteStatus(MMP_UBYTE SRnum, MMP_UBYTE ubStatus);
MMP_ERR MMPF_SF_EnableWrite(void);
MMP_ERR MMPF_SF_EnableWriteSR(void);
MMP_ERR MMPF_SF_DisableWrite(void);
MMP_ERR MMPF_SF_SetIdBufAddr(MMP_ULONG ulAddr);
MMP_ERR MMPF_SF_SetTmpAddr(MMP_ULONG ulStartAddr);
MMP_ERR MMPF_SF_GetTmpAddr(MMP_ULONG *ulStartAddr);
MMP_ERR MMPF_SF_InitialInterface(MMPF_SIF_PAD padid);
MMP_ERR MMPF_SF_Reset(void);
MMPF_SIF_INFO *MMPF_SF_GetSFInfo(void);
MMP_ERR MMPF_SF_ReadDevId(MMP_ULONG *ulDmaAddr);
MMP_ERR MMPF_SF_ReadData(MMP_ULONG ulSFAddr, MMP_ULONG ulDmaAddr, MMP_ULONG ulByteCount);
MMP_ERR MMPF_SF_FastReadData(MMP_ULONG ulSFAddr, MMP_ULONG ulDmaAddr, MMP_ULONG ulByteCount);
MMP_ERR MMPF_SF_WriteData(MMP_ULONG ulSFAddr, MMP_ULONG ulDmaAddr, MMP_ULONG ulByteCount);
MMP_ERR MMPF_SF_EraseSector(MMP_ULONG ulSFAddr);
MMP_ERR MMPF_SF_EraseBlock(MMP_ULONG ulSFAddr);
MMP_ERR MMPF_SF_EraseChip(void);
MMP_ERR MMPF_SF_AaiWriteData(MMP_ULONG ulSFAddr, MMP_ULONG ulDmaAddr, MMP_ULONG ulByteCount);
MMP_ERR MMPF_SF_EBSY(void);
MMP_ERR MMPF_SF_DBSY(void);
MMP_ERR MMPF_SF_ReleasePowerDown(void);
MMP_ERR MMPF_SF_PowerDown(void);
MMP_ERR MMPF_SF_ReadUniqueId(MMP_ULONG64 *ulDmaAddr);
#if (CHIP == MCR_V2)
void MMPF_SF_SetAddrByteLength(MMP_UBYTE bytes);
MMP_ERR MMPF_SF_EnableCrcCheck(MMP_BOOL bEnable);
MMP_USHORT MMPF_SF_GetCrc(void);
#endif

#endif //_MMPF_SF_H_

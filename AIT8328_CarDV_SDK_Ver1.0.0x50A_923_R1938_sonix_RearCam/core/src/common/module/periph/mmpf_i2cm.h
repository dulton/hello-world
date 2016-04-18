//==============================================================================
//
//  File        : mmpf_i2cm.h
//  Description : INCLUDE File for the Firmware I2CM Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_I2CM_H_
#define _MMPF_I2CM_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "mmp_i2c_inc.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define I2CM_RX_FIFO_DEPTH		(0x20)
#define I2CM_TX_FIFO_DEPTH		(0x20)
#define I2CM_SW_MAX_SPEED		(200000) // For CPU 144 MHz, for loop delay set as 1, the one clock cycle is about 53 us

#define I2CM_DEBUG_LEVEL		(0)

#define I2CM_SEM_TIMEOUT 		(0x0)
#define I2CM_WHILE_TIMEOUT      (10000) // Unit:ms
#define I2CM_WHILE_SHORTTIMEOUT (1000)  // Unit:ms

#define I2CM_INT_MODE_EN		(0)		// I2CM Interrupt mode enable or not, 1 stands enable
                                        // Note: I2CM interrupt mode have worse performance and user should take care the risk of i2cm read

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

// Software I2cm
MMP_ERR MMPF_SwI2cm_Start(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_SwI2cm_Stop(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_SwI2cm_WriteData(MMP_I2CM_ATTR *pI2cmAttr, MMP_UBYTE ubData);
MMP_UBYTE MMPF_SwI2cm_ReadData(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_SwI2cm_GetACK(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_SwI2cm_SendNACK(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_SwI2cm_SendACK(MMP_I2CM_ATTR *pI2cmAttr);

// Common I2cm
MMP_ERR MMPF_I2cm_InitDriver(void);
MMP_UBYTE MMPF_I2cm_StartSemProtect(MMP_I2CM_ATTR *pI2cmAttr);
MMP_UBYTE MMPF_I2cm_EndSemProtect(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_I2cm_Initialize(MMP_I2CM_ATTR *pI2cmAttr);
MMP_ERR MMPF_I2cm_SetRxTimeout(MMP_I2CM_ATTR *pI2cmAttr, MMP_ULONG ulTimeOut);
MMP_ERR	MMPF_I2cm_WriteReg(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT usReg, MMP_USHORT usData);
MMP_ERR MMPF_I2Cm_ReadEEDID(MMP_I2CM_ATTR *pI2cmAttr, MMP_UBYTE *ubData, MMP_USHORT usSeg, MMP_USHORT usOffset, MMP_USHORT usSize);
MMP_ERR	MMPF_I2cm_ReadReg(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT usReg, MMP_USHORT *usData);
MMP_ERR	MMPF_I2cm_WriteRegSet(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT *usReg, MMP_USHORT *usData, MMP_UBYTE usSetCnt);
MMP_ERR	MMPF_I2cm_ReadRegSet(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT *usReg, MMP_USHORT *usData, MMP_UBYTE usSetCnt);
MMP_ERR MMPF_I2cm_WriteBurstData(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT usReg, MMP_USHORT *usData, MMP_UBYTE usDataCnt);
MMP_ERR MMPF_I2cm_ReadBurstData(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT usReg, MMP_USHORT *usData, MMP_UBYTE usDataCnt);
MMP_ERR MMPF_I2cm_WriteNoRegMode(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT usData);
MMP_ERR MMPF_I2cm_ReadNoRegMode(MMP_I2CM_ATTR *pI2cmAttr, MMP_USHORT *usData, MMP_UBYTE usDataCnt);
MMP_ERR MMPF_I2cm_DMAWriteBurstData(MMP_I2CM_ATTR   *pI2cmAttr, 
                                    MMP_USHORT      usReg, 
                                    MMP_UBYTE       *usData,
                                    MMP_USHORT      usDataCnt);
MMP_ERR MMPF_I2cm_DMAReadBurstData(MMP_I2CM_ATTR    *pI2cmAttr,
                                   MMP_USHORT       usReg, 
                                   MMP_UBYTE        *usData,
                                   MMP_USHORT       usDataCnt);

#endif // _MMPF_I2CM_H_

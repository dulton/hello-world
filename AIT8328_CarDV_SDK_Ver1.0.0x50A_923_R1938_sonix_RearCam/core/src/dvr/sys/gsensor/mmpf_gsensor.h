//==============================================================================
//
//  File        : mmpf_Gsensor.h
//  Description : INCLUDE File for the Gsensor Driver Function
//  Author      : Mark Chang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_GSENSOR_H_
#define _MMPF_GSENSOR_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmpf_i2cm.h"

/** @addtogroup MMPF_Gsensor
@{
*/

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _GSENSOR_DATA
{
	MMP_UBYTE 	acc_val[3];	//X,Y,Z axis acceleration value
} GSENSOR_DATA;

typedef struct  _GSNR_ATTRIBUTE
{
    MMP_UBYTE ubStoreFreq;
    MMP_UBYTE ubRange;
    MMP_UBYTE ubSensitive;
    MMP_UBYTE ubGroundAxis;
    MMP_UBYTE ub1G;
    MMP_UBYTE ub1G_Err;
    int		  nMaxG;
    int		  nInited;
    MMP_UBYTE ubGsensorID;    // CarDV: Move ubGsensorID to tail of GSNR_ATTRIBUTE to be compatible with GVP player
} GSNR_ATTRIBUTE, *PGSNR_ATTRIBUTE;

typedef struct  _GSNR_I2CM_ATTRIBUTE
{ 
   MMP_I2CM_ID      i2cmID;				//MMP_I2CM0 ~ MMP_I2CM3 stand for HW I2CM
   MMP_UBYTE 		ubPadNum;      		//HW pad map, the relate pad definition, please refer global register spec.
   MMP_ULONG 		ulI2cmSpeed; 			//SW and HW I2CM speed control, the unit is HZ.
   MMP_GPIO_PIN 	sw_clk_pin;  		//Used in SW I2CM (i2cmID = MMP_I2CM_GPIO only), indicate the clock pin
   MMP_GPIO_PIN 	sw_data_pin;	 	//Used in SW I2CM (i2cmID = MMP_I2CM_GPIO only), indicate the data pin
} GSNR_I2CM_ATTRIBUTE;

typedef struct _3RD_PARTY_GSENSOR 
{
    //0
    MMP_ERR (*MMPF_Gsensor_Reset)(struct _3RD_PARTY_GSENSOR *);
    MMP_ERR (*MMPF_Gsensor_ReadDeviceID)(struct _3RD_PARTY_GSENSOR *, GSNR_ATTRIBUTE *pgsnr_attr);    
    MMP_ERR (*MMPF_Gsensor_Initialize)(struct _3RD_PARTY_GSENSOR *, GSNR_ATTRIBUTE *pgsnr_attr);
    MMP_ERR (*MMPF_Gsensor_SetReg)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE usAddr, MMP_UBYTE usData);
    MMP_ERR (*MMPF_Gsensor_GetReg)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE usAddr, MMP_UBYTE *usData);

    //5
    MMP_ERR (*MMPF_Gsensor_ReadOffsetFromFile)(struct _3RD_PARTY_GSENSOR *);
    MMP_ERR (*MMPF_Gsensor_WriteOffsetToFile)(struct _3RD_PARTY_GSENSOR *);
    MMP_ERR (*MMPF_Gsensor_GetSensorOffset)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE *data);
    MMP_ERR (*MMPF_Gsensor_SetSensorOffset)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE *data);
    MMP_ERR (*MMPF_Gsensor_Read_XYZ)(struct _3RD_PARTY_GSENSOR *, GSENSOR_DATA *data);

    //10
    MMP_ERR (*MMPF_Gsensor_Read_XYZ_WithCompensation)(struct _3RD_PARTY_GSENSOR *, GSENSOR_DATA *data);
    MMP_ERR (*MMPF_Gsensor_Read_Accel_Avg)(struct _3RD_PARTY_GSENSOR *, GSENSOR_DATA *data);
    MMP_ERR (*MMPF_Gsensor_CalculateOffset)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE gAxis, GSENSOR_DATA avg);
    MMP_ERR (*MMPF_Gsensor_Calibration)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE side);
    MMP_ERR (*MMPF_Gsensor_SetIntDuration)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE arg);

    //15
    MMP_ERR (*MMPF_Gsensor_GetIntStatus)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE* pstatus);
    MMP_ERR (*MMPF_Gsensor_SetPowerSaveMode)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE mode);
    MMP_ERR (*MMPF_Gsensor_SetNormalModeDataRate)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE rate);
    MMP_ERR (*MMPF_Gsensor_SetInterruptSource)(struct _3RD_PARTY_GSENSOR *, MMP_UBYTE src);
    MMP_ERR (*MMPF_Gsensor_GetDataRate)(struct _3RD_PARTY_GSENSOR *, MMP_USHORT *wDataRate);

    //20
    MMP_ERR (*MMPF_Gsensor_SetDynamicRange)(struct _3RD_PARTY_GSENSOR *, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE ubDrange);
    MMP_ERR (*MMPF_Gsensor_GetDynamicRange)(struct _3RD_PARTY_GSENSOR *, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE *ubDrange);
    MMP_ERR (*MMPF_Gsensor_SetGroundAxis)(struct _3RD_PARTY_GSENSOR *, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE ubDrange);
    MMP_ERR (*MMPF_Gsensor_IOControl)(struct _3RD_PARTY_GSENSOR *, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE cmd, MMP_UBYTE* arg);
    MMP_BOOL (*MMPF_Gsensor_ModuleAttached)(struct _3RD_PARTY_GSENSOR *);
    
    //25
    MMP_ERR (*MMPF_Gsensor_Config_I2C)(struct _3RD_PARTY_GSENSOR *, GSNR_I2CM_ATTRIBUTE *pgsnr_i2cm_attr);
        
} THIRD_PARTY_GSENSOR;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_3RDParty_Gsensor_Config_I2C(GSNR_I2CM_ATTRIBUTE *psI2cmAttr_gsensor);
void MMPF_3RDParty_Gsensor_initiated(MMP_BOOL *bgsnr_inited);

MMP_ERR MMPF_3RDParty_Gsensor_ReadRegister(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE reg , MMP_UBYTE* pval);
MMP_ERR MMPF_3RDParty_Gsensor_WriteRegister(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE reg , MMP_UBYTE val);
MMP_ERR MMPF_3RDParty_Gsensor_ReadOffsetFromFile(void);
MMP_ERR MMPF_3RDParty_Gsensor_WriteOffsetToFile(void);
MMP_ERR MMPF_3RDParty_Gsensor_ResetDevice(void);
MMP_ERR MMPF_3RDParty_Gsensor_ReadDeviceID(void);
MMP_ERR	MMPF_3RDParty_Gsensor_GetSensorOffset(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE *data);
MMP_ERR	MMPF_3RDParty_Gsensor_SetSensorOffset(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE *data);
MMP_ERR MMPF_3RDParty_Gsensor_Read_XYZ(/*struct _3RD_PARTY_GSENSOR *pthis,*/ GSENSOR_DATA *data); 
MMP_ERR MMPF_3RDParty_Gsensor_Read_XYZ_WithCompensation(/*struct _3RD_PARTY_GSENSOR *pthis,*/ GSENSOR_DATA *data);
MMP_ERR MMPF_3RDParty_Gsensor_Read_Accel_Avg(/*struct _3RD_PARTY_GSENSOR *pthis,*/ GSENSOR_DATA *data);
MMP_ERR MMPF_3RDParty_Gsensor_CalculateOffset(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE gAxis, GSENSOR_DATA avg);
MMP_ERR	MMPF_3RDParty_Gsensor_Calibration(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE side);
MMP_ERR	MMPF_3RDParty_Gsensor_SetIntDuration(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE arg);
MMP_ERR MMPF_3RDParty_Gsensor_GetIntStatus(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE* pstatus);
MMP_ERR MMPF_3RDParty_Gsensor_SetPowerSaveMode(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE mode);
MMP_ERR MMPF_3RDParty_Gsensor_SetNormalModeDataRate(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE rate);
MMP_ERR MMPF_3RDParty_Gsensor_SetInterruptSource(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE src);
MMP_ERR MMPF_3RDParty_Gsensor_GetDataRate(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_USHORT *wDataRate);
MMP_ERR MMPF_3RDParty_Gsensor_SetDynamicRange(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE ubDrange);
MMP_ERR MMPF_3RDParty_Gsensor_GetDynamicRange(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE *ubDrange);
MMP_ERR MMPF_3RDParty_Gsensor_SetGroundAxis(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE arg);
MMP_ERR MMPF_3RDParty_Gsensor_Initial(void);
MMP_ERR MMPF_3RDParty_Gsensor_IOControl(/*struct _3RD_PARTY_GSENSOR *pthis,*/ MMP_UBYTE cmd, MMP_UBYTE* arg);
MMP_BOOL MMPF_3RDParty_Gsensor_Attached(void);

void MMPF_3RDParty_Gsensor_Register(struct _3RD_PARTY_GSENSOR *pdev_obj);
void MMPF_3RDParty_Gsensor_Unregister(void);

GSNR_ATTRIBUTE *MMPF_3RDParty_Gsensor_GetAttribute(void);

#endif // _MMPF_GSENSOR_H_

/// @}

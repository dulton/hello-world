#ifndef _AHC_IR_H_
#define _AHC_IR_H_

/*===========================================================================
 * Include files
 *===========================================================================*/ 
#include "Customer_Config.h"

#include "AHC_Common.h"
#include "AHC_General.h" 
#include "mmpf_i2cm.h"

#define AHC_IR_RET_FALSE    (AHC_FALSE)
#define AHC_IR_RET_TRUE     (AHC_TRUE)
#define AHC_IR_NO_ERROR     (MMP_ERR_NONE)
#define AHC_IR_CHECK_RETURE_VALUE(RET_VAL, NO_ERROR, YES, NO) return (RET_VAL == NO_ERROR) ? YES : NO;

typedef struct
{
	UINT8 	data[4];	
	UINT8 	RepeatTimes;	
} AHC_IR_DATA;

typedef struct _AHC_IR_I2CM_ATTRIBUTE
{
   MMP_I2CM_ID  i2cmID;             //MMP_I2CM0 ~ MMP_I2CM2 stand for HW I2CM
   UINT8 	 	ubPadNum;      		//HW pad map, the relate pad definition, please refer global register spec.
   UINT32	 	ulI2cmSpeed;        //SW and HW I2CM speed control, the unit is HZ.
  
} AHC_IR_I2CM_ATTRIBUTE;

/*===========================================================================
 * Function prototype
 *===========================================================================*/
AHC_BOOL AHC_IR_Config_I2C(IR_I2CM_ATTRIBUTE *psI2cmAttr_gsensor);
AHC_BOOL AHC_IR_ReadRegister(UINT16 reg,  UINT16* pval);
AHC_BOOL AHC_IR_WriteRegister(UINT16 reg, UINT16 pval);
AHC_BOOL AHC_IR_ModuleAttached(void);
AHC_BOOL AHC_IR_GetIrData(IR_DATA *pData);
AHC_BOOL AHC_IR_GetIntStatus(UINT8 *pstatus);
AHC_BOOL AHC_IR_Initial(void);

#endif

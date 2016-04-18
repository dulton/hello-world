//==============================================================================
//
//  File        : AHC_Gsensor.c
//  Description : AHC Gsensor control function
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h" 
#include "mmpf_gsensor.h"
#include "AHC_Gsensor.h"
#include "Gsensor_Ctrl.h"

/*===========================================================================
 * Main body
 *===========================================================================*/
AHC_BOOL AHC_Gsensor_Module_Attached(void)
{	
    return MMPF_3RDParty_Gsensor_Attached();
}

AHC_BOOL AHC_Gsensor_Config_I2C(AHC_GSENSOR_I2CM_ATTRIBUTE *psI2cmAttr_gsensor)
{	
    return MMPF_3RDParty_Gsensor_Config_I2C((GSNR_I2CM_ATTRIBUTE *)psI2cmAttr_gsensor);
}

AHC_BOOL AHC_Gsensor_Initial(void)
{       
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_Initial();
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_ReadRegister(UINT16 reg , UINT16* pval)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_ReadRegister((MMP_UBYTE)reg, (MMP_UBYTE *)pval );
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_WriteRegister(UINT16 reg , UINT16 pval)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_WriteRegister((MMP_UBYTE)reg, (MMP_UBYTE)pval);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}


AHC_BOOL AHC_Gsensor_ReadOffsetFromFile(void)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_ReadOffsetFromFile();
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_WriteOffsetToFile(void)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_WriteOffsetToFile();
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_ResetDevice(void)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_ResetDevice();
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_ReadDeviceID(void)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_ReadDeviceID();
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_GetSensorOffset(UINT8 *data)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_GetSensorOffset(data);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetSensorOffset(UINT8 *data)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetSensorOffset(data);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_Gsensor_Read_XYZ(AHC_GSENSOR_DATA *data)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_Read_XYZ((GSENSOR_DATA *)data);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_Read_XYZ_WithCompensation(AHC_GSENSOR_DATA *data)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_Read_XYZ_WithCompensation((GSENSOR_DATA *)data);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_Read_Accel_Avg(AHC_GSENSOR_DATA *data)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_Read_Accel_Avg((GSENSOR_DATA *)data);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_CalculateOffset(UINT8 gAxis, GSENSOR_DATA avg)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_CalculateOffset(gAxis, avg);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_Calibration(UINT8 side)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_Calibration( side);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetIntDuration(UINT8 arg)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetIntDuration(arg);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_GetIntStatus(UINT8* pstatus)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_GetIntStatus(pstatus);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetPowerSaveMode(UINT8 mode)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetPowerSaveMode(mode);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetNormalModeDataRate(UINT8 rate)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetNormalModeDataRate(rate);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetInterruptSource(UINT8 src)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetInterruptSource(src);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_GetDataRate(UINT16 *wDataRate)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_GetDataRate(wDataRate);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetDynamicRange(UINT8 ubDrange)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetDynamicRange(ubDrange);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_GetDynamicRange(UINT8 *ubDrange)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_GetDynamicRange(ubDrange);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_SetGroundAxis(UINT8 arg)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_SetGroundAxis(arg);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}

AHC_BOOL AHC_Gsensor_IOControl(UINT8 cmd, void* arg)
{
    MMP_ERR ret_val;
    ret_val = MMPF_3RDParty_Gsensor_IOControl((MMP_UBYTE)cmd, (MMP_UBYTE*)arg);
    AHC_GSENSOR_CHECK_RETURE_VALUE(ret_val, AHC_GSENSOR_NO_ERROR, AHC_GSENSOR_RET_TRUE, AHC_GSENSOR_RET_FALSE)
}


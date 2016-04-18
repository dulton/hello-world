
#include "customer_config.h"
#include "IR_Ctrl.h"
#include "AHC_Common.h" 
#include "AHC_IR.h"

AHC_BOOL AHC_IR_Config_I2C(IR_I2CM_ATTRIBUTE *psI2cmAttr_gsensor)
{	
    return IR_Ctrl_Config_I2C(psI2cmAttr_gsensor);
}

AHC_BOOL AHC_IR_ReadRegister(UINT16 reg , UINT16* pval)
{
    MMP_ERR ret_val;
    ret_val = IR_Ctrl_ReadRegister((MMP_UBYTE)reg, (MMP_UBYTE *)pval );
    AHC_IR_CHECK_RETURE_VALUE(ret_val, AHC_IR_NO_ERROR, AHC_IR_RET_TRUE, AHC_IR_RET_FALSE)
}

AHC_BOOL AHC_IR_WriteRegister(UINT16 reg , UINT16 pval)
{
    MMP_ERR ret_val;
    ret_val = IR_Ctrl_WriteRegister((MMP_UBYTE)reg, (MMP_UBYTE)pval);
    AHC_IR_CHECK_RETURE_VALUE(ret_val, AHC_IR_NO_ERROR, AHC_IR_RET_TRUE, AHC_IR_RET_FALSE)
}

AHC_BOOL AHC_IR_ModuleAttached(void)
{	
    return IR_Ctrl_Attached();
}

AHC_BOOL AHC_IR_GetIrData(IR_DATA *pData)
{
    MMP_ERR ret_val;
    ret_val = IR_Ctrl_GetIrData(pData);
    AHC_IR_CHECK_RETURE_VALUE(ret_val, AHC_IR_NO_ERROR, AHC_IR_RET_TRUE, AHC_IR_RET_FALSE)
}

AHC_BOOL AHC_IR_GetIntStatus(UINT8* pstatus)
{
    MMP_ERR ret_val;
    ret_val = IR_Ctrl_GetIntStatus(pstatus);
    AHC_IR_CHECK_RETURE_VALUE(ret_val, AHC_IR_NO_ERROR, AHC_IR_RET_TRUE, AHC_IR_RET_FALSE)
}

AHC_BOOL AHC_IR_Initial(void)
{       
    MMP_ERR ret_val;
    ret_val = IR_Ctrl_Initial();
    AHC_IR_CHECK_RETURE_VALUE(ret_val, AHC_IR_NO_ERROR, AHC_IR_RET_TRUE, AHC_IR_RET_FALSE)
}


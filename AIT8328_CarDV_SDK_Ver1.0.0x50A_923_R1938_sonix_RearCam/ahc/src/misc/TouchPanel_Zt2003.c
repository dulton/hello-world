/*===========================================================================
 * Include files
 *===========================================================================*/

#include "mmph_hif.h"
#include "mmpf_pio.h"
#include "mmpf_i2cm.h"
#include "TouchPanel_Zt2003.h"
#include "AIT_init.h"
#include "AHC_utility.h"
#include "mmpf_touchpanel.h"
#include "mmp_reg_pad.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#if ( DSC_TOUCH_PANEL == TOUCH_PANEL_ZT2003 )
//      MSB                                                     LSB
//  -|-------|-------|-------|-------|-------|-------|-------|-------|
//   |  C3   |  C2   |  C1   |  C0   |  PD1  |  PD0  |   M0  |  X    |
//  -|-------|-------|-------|-------|-------|-------|-------|-------|
//
//  C3~C1    - Input Config
//  PD1~PD0  - Power down bit functions.
//  X        - Don't care 
//
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  C3 |  C2 |  C1 |  C0 |  Function                  | Input to ADC | X Drivers  | Y Drivers  | Reference Mode  |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  0  |  0  |  Measure TEMP0             |     TEMP0    |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  0  |  1  |  Measure VBAT1             |     VBAT1    |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  1  |  0  |  Measure IN1               |     IN1      |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  1  |  1  |  Reserved                  |              |            |            |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  0  |  0  |  Measure TEMP1             |     TEMP1    |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  0  |  1  |  Measure VBAT2             |     VBAT2    |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  1  |  0  |  Measure IN2               |     IN2      |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  1  |  1  |  Reserved                  |              |            |            |  Single Ended   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  0  |  0  |  Activate X+, X  Drivers   |              |    ON      |    OFF     |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  0  |  1  |  Activate Y+, Y  Drivers   |              |    OFF     |    ON      |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  1  |  0  |  Activate Y+, X  Drivers   |              |   X  ON    |   Y+ ON    |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  1  |  1  |  Reserved                  |              |            |            |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  0  |  0  |  Measure X position        |      Y+      |    ON      |    OFF     |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  0  |  1  |  Measure Y Position        |      X+      |    OFF     |    ON      |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  1  |  0  |  Measure Z1 Position       |      X+      |   X  ON    |   Y+ ON    |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  1  |  1  |  Measure Z2 Position       |      Y       |   X  ON    |   Y+ ON    |  Differential   |
// |-----|-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
//
//
//  |-------|-------|-----------|-----------------------------------|
//  |  PD1  |  PD0  | ^PENIRQ   | DESCRIPTION                       |
//  |-------|-------|-----------|-----------------------------------|
//  |   0   |   0   | Enabled   |  Power Down between conversions   |
//  |-------|-------|-----------|-----------------------------------|
//  |   0   |   1   | Disable   |  Internal reference OFF, ADC ON   |
//  |-------|-------|-----------|-----------------------------------|
//  |   1   |   0   | Enabled   |  Internal reference ON, ADC OFF   |
//  |-------|-------|-----------|-----------------------------------|
//  |   1   |   1   | Disable   |  Internal reference ON, ADC ON    |
//  |-------|-------|-----------|-----------------------------------|
//
//  |-------|-----------------|
//  |   M   | DESCRIPTION     |
//  |-------|-----------------|
//  |   0   |  12 bit mode    |
//  |-------|-----------------|
//  |   1   |   8 bit mode    |
//  |-------|-----------------|
//
// Config bits
#define TOUCH_SCREEN_CMD_READ_X 			     	0xD0
#define TOUCH_SCREEN_CMD_READ_Y				     	0xC0
#define TOUCH_SCREEN_CMD_READ_Z1 			     	0xE0
#define TOUCH_SCREEN_CMD_READ_Z2 			     	0xF0
#define TOUCH_SCREEN_CMD_READ_VBAT1 			 	0x10

// PD1 PD0 bits
#define TOUCH_SCREEN_CMD_IRQ_AND_PWR_DOWN       	0x00
#define TOUCH_SCREEN_CMD_INTERNAL_OFF_ADC_ON 		0x04
#define TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF 	0x08
#define TOUCH_SCREEN_CMD_INTERNAL_REF_ON_ADC_ON  	0x0C

// Resolution bit - M bit (bit 1)
#define TOUCH_SCREEN_CMD_RESOLUTION_12BIT        	0x00
#define TOUCH_SCREEN_CMD_RESOLUTION_8BIT         	0x02

#define MMP_GPIO_PULL_HIGH                       	0x24

#define TOUCH_PANEL_I2C_SLAVE_ADDRESS 				0x90 >> 1 // for AIT I2C HW setting, slave address 0x90 should shif 1 bit (>> 1)

/*===========================================================================
 * Global varible
 *===========================================================================*/

static MMP_I2CM_ATTR  m_TouchPanelI2c = {MMP_I2CM2, TOUCH_PANEL_I2C_SLAVE_ADDRESS, 0, 8, 0x20, MMP_FALSE, MMP_FALSE, MMP_FALSE, MMP_FALSE, 0x0, 0x0, 0x0, 200000 /*250KHZ*/, MMP_TRUE, NULL, NULL, MMP_TRUE, MMP_TRUE, MMP_FALSE, 0};

/*===========================================================================
 * Main body
 *===========================================================================*/

MMP_ERR TouchPanel_Init(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
    MMP_ERR     status      = MMP_ERR_NONE;
    AITPS_PAD  pPAD   = AITC_BASE_PAD;

    MMP_USHORT usWriteData = TOUCH_SCREEN_CMD_READ_X|
                             TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                             TOUCH_SCREEN_CMD_RESOLUTION_12BIT;

    // Interrupt pin. Need to pull high to active.
    // Low   - Touch down
    // Hight - No touch
    pPAD->PAD_IO_CFG_PCGPIO[0x1B] = MMP_GPIO_PULL_HIGH; // CGPIO27 = GPIO59

    // Write PD0 = 0, to enable interrupt pin.
	status = MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);

    return status;
}

void TouchPanel_TransformPosition(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir) // Set dir = 0 as lcd normal, 1: reverse.
{
	#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
	
	MMP_USHORT tpRefCenterX = 2098, tpRefCenterY = 2099, tpStepX = 12, tpStepY = 14;

	#if (TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_0)
	if(dir)//LCD reverse
	{
		*pX = ZT2003_ADC_STEPS-*pX;
		*pY = ZT2003_ADC_STEPS-*pY;
	}
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_1)
	if(dir)
	{
    	*pX = ZT2003_ADC_STEPS-*pX;
	}
    else
    {
        *pY = ZT2003_ADC_STEPS-*pY;
    }
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_2)
	if(dir)
	{
    	*pY = ZT2003_ADC_STEPS-*pY;
	}
    else
    {
        *pX = ZT2003_ADC_STEPS-*pX;
    }
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_3)
	if(!dir)//LCD Normal
	{
		*pX = ZT2003_ADC_STEPS-*pX;
		*pY = ZT2003_ADC_STEPS-*pY;
	}
	#endif

	if (*pY > tpRefCenterY)
	{ 
		*pY =  120+(*pY-tpRefCenterY)/tpStepY;
		
		if(*pY > 240)
		*pY = 240;
	}
	else
	{	
		if((tpRefCenterY-*pY)/tpStepY < 120)
		{
			*pY = 120-(tpRefCenterY-*pY)/tpStepY;
		}
		else
		{
			*pY = 0;
		}
	}
	
	if (*pX > tpRefCenterX)
	{ 
		*pX =  160+(*pX-tpRefCenterX)/tpStepX;
		
		if(*pY > 320)
		*pY = 320;
	}
	else
	{	
		if((tpRefCenterX-*pX)/tpStepX < 160)
		{
			*pX =  160-(tpRefCenterX-*pX)/tpStepX;
		}
		else
		{
			*pX = 0;
		}
	}
	#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
	
	MMP_USHORT tpRefCenterX = 2160, tpRefCenterY = 2030, tpStepX = 11, tpStepY = 15;
	
	if (*pY > tpRefCenterY)
	{ 
		*pY =  (*pY-tpRefCenterY)/tpStepY;

		if(*pY > 120)
			*pY = 0;
		else
			*pY = 120 - *pY;
	}
	else
	{	
		if((tpRefCenterY-*pY)/tpStepY < 120)
		{
			*pY = 120 + (tpRefCenterY-*pY)/tpStepY;
		}
		else
		{
			*pY = 240;
		}
	}
	
	if (*pX > tpRefCenterX)
	{ 
		*pX =  160+(*pX-tpRefCenterX)/tpStepX;
		
		if(*pX > 320)
		*pX = 320;
	}
	else
	{
		if((tpRefCenterX-*pX)/tpStepX < 160)
		{
			*pX =  160-(tpRefCenterX-*pX)/tpStepX;
		}
		else
		{
			*pX = 0;
		}
	}
	
	if (dir)
	{ 
		*pY =  240 - *pY;
	}
	#endif///< (VERTICAL_LCD == 0)
}
	
int TouchPanel_ReadPosition(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir)
{
    MMP_USHORT usReadData[4] = {0};
    MMP_USHORT usWriteData = 0;
    MMP_UBYTE  Temp[4];
    MMP_ERR ret= 0;

	// X    
	usWriteData = TOUCH_SCREEN_CMD_READ_X|
                  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;

    ret = MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);
    ret = MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, NULL, usReadData, 0x2);

    Temp[0] = usReadData[0];
    Temp[1] = usReadData[1];
    *pX = Temp[0]*16+Temp[1]/16;

	// Y
	usWriteData = TOUCH_SCREEN_CMD_READ_Y|
                  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;
    
    MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);
    MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, NULL, usReadData, 0x2);
	
	Temp[0] = usReadData[0];
    Temp[1] = usReadData[1];

    *pY = Temp[0]*16+Temp[1]/16;

	// Z1
	usWriteData = TOUCH_SCREEN_CMD_READ_Z1|
                  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;
	
    MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);
    MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, NULL, usReadData, 0x2);
	
	TouchPanel_TransformPosition(pX,pY,dir);
	//printc("pX %d  pY %d\r\n", *pX,  *pY);
    if( usReadData[0] == 0 )
    {
	    *pX = 0;
	    *pY = 0;
    	return 0;
    }
	
	return 1;
}


MMP_ERR TouchPanel_ReadVBAT1(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir )
{
    MMP_ERR     status      = MMP_ERR_NONE;
    MMP_USHORT usReadData[4] = {0};
    MMP_USHORT usWriteData = 0;
    MMP_UBYTE  Temp[4];
    
	usWriteData = TOUCH_SCREEN_CMD_READ_VBAT1|
				  TOUCH_SCREEN_CMD_INTERNAL_REF_ON_ADC_ON |
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;
    
	status |= MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);
	status |= MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, NULL, usReadData, 0x2);
     
    Temp[0] = usReadData[0];
    Temp[1] = usReadData[1];
    
    *pX = Temp[0]*16+Temp[1]/16;
    
    printc("ADC Value: %d\r\n",*pX);
	
	*pX = *pX * (2625) / 1000;
	
	printc("FINAL : %d mvolt\r\n",*pX);
    
	status |= MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, TOUCH_SCREEN_CMD_IRQ_AND_PWR_DOWN);
    
	return status;
}
void TouchPanel_ResetCtrl(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_BOOL bOn)
{
#ifdef TOUCH_RESET_GPIO
	if (TOUCH_RESET_GPIO != MMP_GPIO_MAX)
	{
		MMPF_PIO_EnableOutputMode(TOUCH_RESET_GPIO, MMP_TRUE, MMP_TRUE);
		MMPF_PIO_SetData(TOUCH_RESET_GPIO, (bOn ? TOUCH_RESET_GPIO_ACT_MODE : !TOUCH_RESET_GPIO_ACT_MODE), MMP_TRUE);
	}
#endif

}

MMP_BOOL TouchPanel_Attached(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
return AHC_TRUE;
}

MMP_ERR TouchPanel_IOControl(struct _3RD_PARTY_TOUCHPANEL *pthis, TOUCHPANEL_ATTR *ptouchpanel_attr, MMP_UBYTE cmd, MMP_UBYTE* arg)
{
    MMP_ERR status = MMP_ERR_NONE;

return status;
}

struct _3RD_PARTY_TOUCHPANEL TouchPanel_Obj =
{
    //0
        TouchPanel_Init,
        //TouchPanel_CheckUpdate,
        TouchPanel_ReadPosition,
        TouchPanel_ReadVBAT1,
    //    TouchPanel_TransformPosition,
        TouchPanel_ResetCtrl,

    //5

        TouchPanel_IOControl,
        TouchPanel_Attached,

};

int Touch_Panel_Module_Init(void)
{
    printc("[Touch_Panel_Module_Init]\r\n");
    MMPF_3RDParty_TouchPanel_Register(&TouchPanel_Obj);
    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6"
#pragma O0
ait_module_init(Touch_Panel_Module_Init);
#pragma
#pragma arm section rodata, rwdata ,  zidata

#endif //TOUCH_PANEL_ZT2003

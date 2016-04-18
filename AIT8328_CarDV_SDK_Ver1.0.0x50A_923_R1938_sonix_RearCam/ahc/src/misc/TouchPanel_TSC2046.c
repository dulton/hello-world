/*===========================================================================
 * Include files
 *===========================================================================*/

#include "mmph_hif.h"
#include "mmpf_pio.h"
//#include "mmpf_i2cm.h"
#include "mmpf_spi.h"
#include "TouchPanel_TSC2046.h"
#include "AIT_init.h"
#include "AHC_utility.h"
#include "mmpf_touchpanel.h"
#include "mmp_reg_pad.h"
/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#if ( DSC_TOUCH_PANEL == TOUCH_PANEL_TSC2046 )

//      MSB                                                     LSB
//  -|-------|-------|-------|-------|-------|------------|-------|-------|
//   |   S   |  A2   |  A1   |  A0   |  MODE |  SER/^DFR  |  PD1  |  PD0  |
//  -|-------|-------|-------|-------|-------|------------|-------|-------|
//
//
//  A2-A0
//  PD1~PD0  - Power down bit functions.
//
//
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  A2 |  A1 |  A0 |  Function                  | Input to ADC | X Drivers  | Y Drivers  | Reference Mode  |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  0  |  Measure TEMP0             |     TEMP0    |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  1  |  Reserved                  |              |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  0  |  Measure AUX               |     AUX      |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  1  |  Reserved                  |              |            |            |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  0  |  Measure TEMP1             |     TEMP1    |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  1  |  Reserved                  |              |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  0  |  Reserved                  |              |    OFF     |    OFF     |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  1  |  Reserved                  |              |            |            |  Single Ended   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
//Setup Command    Activate Y-Drivers  Activate X-Drivers   Activate Y+, X- Drivers  ON  OFF

// |  0  |  0  |  0  |                            |              |            |            |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  0  |  1  |  Measure Y Position        |      X+      |    OFF     |    ON      |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  0  |                            |              |   X- ON    |   Y+ ON    |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  0  |  1  |  1  |  Measure Z1 Position       |              |    OFF     |    OFF     |                 |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  0  |   Measure Z2 Position      |              |    ON      |    OFF     |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  0  |  1  |  Measure X position        |      Y+      |    ON      |    OFF     |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  0  |                            |              |   X- ON    |   Y+ ON    |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
// |  1  |  1  |  1  |                            |              |   X- ON    |   Y+ ON    |  Differential   |
// |-----|-----|-----|----------------------------|--------------|------------|------------|-----------------|
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
//  |  Mode | DESCRIPTION     |
//  |-------|-----------------|
//  |   0   |  12 bit mode    |
//  |-------|-----------------|
//  |   1   |   8 bit mode    |
//  |-------|-----------------|
//
// Config bits
#define TOUCH_SCREEN_CMD_READ_X 			     	0x10
#define TOUCH_SCREEN_CMD_READ_Y				     	0x50
#define TOUCH_SCREEN_CMD_READ_Z1 			     	0x30
#define TOUCH_SCREEN_CMD_READ_Z2 			     	0x40
//#define TOUCH_SCREEN_CMD_SETUP                      0xB0
// PD1 PD0 bits
#define TOUCH_SCREEN_CMD_IRQ_AND_PWR_DOWN       	0x00
#define TOUCH_SCREEN_CMD_INTERNAL_OFF_ADC_ON 		0x01
#define TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF 	0x02
#define TOUCH_SCREEN_CMD_INTERNAL_REF_ON_ADC_ON  	0x03

// Resolution bit - M bit (bit 1)
#define TOUCH_SCREEN_CMD_RESOLUTION_12BIT        	0x00
#define TOUCH_SCREEN_CMD_RESOLUTION_8BIT         	0x08

//
//#define TOUCH_SCREEN_CMD_PENIRQ_PU_RESISTOR_50K     0x00
//#define TOUCH_SCREEN_CMD_PENIRQ_PU_RESISTOR_90K     0x01

#define MMP_GPIO_PULL_HIGH                       	0x24

//#define TOUCH_PANEL_I2C_SLAVE_ADDRESS 				0x90 >> 1 // for AIT I2C HW setting, slave address 0x90 should shif 1 bit (>> 1)

/*===========================================================================
 * Global varible
 *===========================================================================*/

static MMPF_SPI_ATTRIBUTE  m_TouchPanelSpi = {MMPF_SPI_MASTER_MODE, 0, 24, 65, 0, 0, 0, 0, 0, MMPF_SPI_PAD_1};
MMPF_SPI_ID m_usTouSpiID;
static MMP_UBYTE TP_inited = 0;

/*======================================================================ch====
 * Main body
 *===========================================================================*/

MMP_ERR TouchPanel_Init(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
    MMP_ERR     status      = MMP_ERR_NONE;
    AITPS_PAD  pPAD   = AITC_BASE_PAD;
    AITPS_GBL   pGBL = AITC_BASE_GBL;

    MMP_UBYTE usWriteData[2];


    // Interrupt pin. Need to pull high to active.
    // Low   - Touch down
    // Hight - No touch
    pPAD->PAD_IO_CFG_PCGPIO[0x1F] = MMP_GPIO_PULL_HIGH; // CGPIO31 = GPIO63


//    usWriteData [0] =  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF| 0x80 |
//                                TOUCH_SCREEN_CMD_RESOLUTION_12BIT;//0x02


   //MMP_UBYTE   spi_pad_cfg
    MMPF_SPI_Initialize();
    m_usTouSpiID                  =   MMPF_SPI_PAD_1;

    MMPF_SPI_SetAttributes(m_usTouSpiID, &m_TouchPanelSpi);

    // Write PD0 = 0, to enable interrupt pin.
    //status = MMPF_SPI_Write(m_usTouSpiID, (MMP_ULONG)usWriteData, 1);
    //printc("\r\n\n[H1] ret %d\r\n\n", status);
    TP_inited = 1;


    return status;
}

void TouchPanel_TransformPosition(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir) // Set dir = 0 as lcd normal, 1: reverse.
{

	
	MMP_USHORT tpRefCenterX = 1960, tpRefCenterY = 1945, tpStepX = 7, tpStepY = 10;
	//printc("[H4] \r\n");
	#if (TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_0)
	if(dir)//LCD reverse
	{
		*pX = TSC2046_ADC_STEPS-*pX;
		*pY = TSC2046_ADC_STEPS-*pY;
	}
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_1)
	if(dir)
	{
    	*pX = TSC2046_ADC_STEPS-*pX;
	}
    else
    {
        *pY = TSC2046_ADC_STEPS-*pY;
    }
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_2)
	if(dir)
	{
    	*pY = TSC2046_ADC_STEPS-*pY;
	}
    else
    {
        *pX = TSC2046_ADC_STEPS-*pX;
    }
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_3)
	if(!dir)//LCD Normal
	{
		*pX = TSC2046_ADC_STEPS-*pX;
		*pY = TSC2046_ADC_STEPS-*pY;
	}
	#endif

	if (*pY > tpRefCenterY)
	{ 
		*pY =  160+(*pY-tpRefCenterY)/tpStepY;
		
		if(*pY > 320)
		*pY = 320;
	}
	else
	{	
		if((tpRefCenterY-*pY)/tpStepY < 160)
		{
			*pY = 160-(tpRefCenterY-*pY)/tpStepY;
		}
		else
		{
			*pY = 0;
		}
	}
	
	if (*pX > tpRefCenterX)
	{ 
		*pX =  240+(*pX-tpRefCenterX)/tpStepX;
		
		if(*pY > 480)
		*pY = 480;
	}
	else
	{	
		if((tpRefCenterX-*pX)/tpStepX < 240)
		{
			*pX =  240-(tpRefCenterX-*pX)/tpStepX;
		}
		else
		{
			*pX = 0;
		}
	}

}
	
int TouchPanel_ReadPosition(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir)
{
    MMP_UBYTE usReadData[3] = {0};
    MMP_UBYTE usWriteData[3] = {0};
    MMP_UBYTE  Temp[4];
    MMP_ERR ret= 0;
    //printc("\r\n\n[H2] \r\n\n");
    if (TP_inited == 1)
    {
	// X    
	usWriteData[2] = TOUCH_SCREEN_CMD_READ_X|0x80|
                  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;//0x52

//	MMPF_SPI_Write(m_usSpiID, usWriteData, 1);

	//ret = MMPF_SPI_Read(m_usTouSpiID, (MMP_ULONG)usWriteData, 1 , (MMP_ULONG)usReadData, 2);
	ret = MMPF_SPI_Read(m_usTouSpiID, (MMP_ULONG)usReadData, 3 , (MMP_ULONG)usWriteData, 3);
    Temp[0] = usReadData[0]>>3 + usReadData[1]&0x01;
    Temp[1] = usReadData[1];
    Temp[2] = usReadData[2];
   //printc("tempX %x %x %x ", Temp[0], Temp[1], Temp[2]);
    *pX = (Temp[1]*256+Temp[0])/8;


	// Y
	usWriteData[2] = TOUCH_SCREEN_CMD_READ_Y|0x80|
                  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;//0x12
    

    ret = MMPF_SPI_Read(m_usTouSpiID, (MMP_ULONG)usReadData, 3 , (MMP_ULONG)usWriteData, 3);

	
	Temp[0] = usReadData[0];
    Temp[1] = usReadData[1];
    Temp[2] = usReadData[2];
    //   printc("tempY %x %x %x ", Temp[0], Temp[1], Temp[2]);
    *pY = (Temp[1]*256+Temp[0])/8;

	// Z1
	usWriteData[2] = TOUCH_SCREEN_CMD_READ_Z1|0x80|
                  TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;//0x32
	

    ret = MMPF_SPI_Read(m_usTouSpiID, (MMP_ULONG)usReadData, 3 , (MMP_ULONG)usWriteData, 3);

	Temp[0] = usReadData[0];
	Temp[1] = usReadData[1];

//	Temp[2] = usReadData[2];
//	   printc("tempZ %x %x %x ", Temp[0], Temp[1], Temp[2]);


   //printc("pX %d  pY %d\r\n", *pX,  *pY);
	TouchPanel_TransformPosition(pX,pY,dir);

    if( usReadData[1] == 0 )
    {
        //printc("pX 0  pY 0\r\n");
	    *pX = 0;
	    *pY = 0;
    	return 0;
    }
	
	return 1;
    }
}



MMP_ERR TouchPanel_ReadVBAT1(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir )
{
    MMP_ERR     status      = MMP_ERR_NONE;
//    MMP_USHORT usReadData[4] = {0};
//    MMP_USHORT usWriteData = 0;
//    MMP_UBYTE  Temp[4];
//
//	usWriteData = TOUCH_SCREEN_CMD_READ_VBAT1|
//				  TOUCH_SCREEN_CMD_INTERNAL_REF_ON_ADC_ON |
//                  TOUCH_SCREEN_CMD_RESOLUTION_12BIT;
//
//	status |= MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);
//	status |= MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, NULL, usReadData, 0x2);
//
//    Temp[0] = usReadData[0];
//    Temp[1] = usReadData[1];
//
//    *pX = Temp[0]*16+Temp[1]/16;
//
//    printc("ADC Value: %d\r\n",*pX);
//
//	*pX = *pX * (2625) / 1000;
//
//	printc("FINAL : %d mvolt\r\n",*pX);
//
//	status |= MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, TOUCH_SCREEN_CMD_IRQ_AND_PWR_DOWN);
    
	return status;
}
void TouchPanel_ResetCtrl(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_BOOL bOn)
{
#ifdef TOUCH_RESET_GPIO
	
	if(TOUCH_RESET_GPIO != MMP_GPIO_MAX)
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

#endif //TOUCH_PANEL_TSC2046


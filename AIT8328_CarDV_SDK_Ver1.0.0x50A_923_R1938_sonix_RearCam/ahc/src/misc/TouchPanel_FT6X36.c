/*===========================================================================
 * Include files
 *===========================================================================*/

#include "mmph_hif.h"
#include "mmpf_pio.h"
#include "mmpf_i2cm.h"
#include "TouchPanel_FT6X36.h"
#include "AIT_init.h"
#include "AHC_utility.h"
#include "mmpf_touchpanel.h"
#include "mmp_reg_pad.h"
#include "lcd_common.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#if ( DSC_TOUCH_PANEL == TOUCH_PANEL_FT6X36 )
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

#define TOUCH_PANEL_I2C_SLAVE_ADDRESS 				0x70>>1  // for AIT I2C HW setting, slave address 0x90 should shif 1 bit (>> 1)

/*===========================================================================
 * Global varible
 *===========================================================================*/

static MMP_I2CM_ATTR  m_TouchPanelI2c = {MMP_I2CM2, TOUCH_PANEL_I2C_SLAVE_ADDRESS, 0, 8, 0x20, MMP_FALSE, MMP_FALSE, MMP_FALSE, MMP_FALSE, 0x0, 0x0, 0x0, 200000 /*250KHZ*/, MMP_TRUE, NULL, NULL, MMP_TRUE, MMP_TRUE, MMP_FALSE};
extern MMP_ULONG glAhcBootComplete; 

/*===========================================================================
 * Main body
 *===========================================================================*/

static void InitTouchGpio( MMP_GPIO_PIN piopin,GpioCallBackFunc* CallBackFunc  )
{
    if(piopin == MMP_GPIO_MAX)
		return;

    //Set GPIO as input mode
    MMPF_PIO_EnableOutputMode(piopin, MMP_FALSE, MMP_TRUE);

    // Set the trigger mode.
    MMPF_PIO_EnableTrigMode(piopin, GPIO_H2L_EDGE_TRIG, MMP_TRUE, MMP_TRUE);
//    MMPF_PIO_EnableTrigMode(piopin, GPIO_L2H_EDGE_TRIG, MMP_TRUE, MMP_TRUE);
    
    //Enable Interrupt
    MMPF_PIO_EnableInterrupt(piopin, MMP_TRUE, 0, (GpioCallBackFunc *)CallBackFunc, MMP_TRUE);
}

static 
void TOUCH_ISR(MMP_GPIO_PIN piopin)
{
    MMP_ULONG  ulNow;
	static MMP_ULONG ulPre = 0;
	
    //printc(FG_RED("### TOUCH_ISR ###\r\n"));
    MMPF_OS_GetTime(&ulNow);
	if(ulNow - ulPre < 200)
	{
		ulPre = ulNow;
		return;
	}
	ulPre = ulNow;
	if(glAhcBootComplete == MMP_TRUE)
        SetKeyPadEvent(TOUCH_PANEL_PRESS);
}

MMP_ERR TouchPanel_Init(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
    MMP_ERR         status      = MMP_ERR_NONE;
    AITPS_PAD       pPAD   = AITC_BASE_PAD;
    MMP_USHORT      readtouch[8]={0};
    MMP_USHORT      uc_reg_addr,uc_reg_value;
    MMP_UBYTE       ubSlaveAddr;
    MMP_USHORT      usWriteData = TOUCH_SCREEN_CMD_READ_X|
                             TOUCH_SCREEN_CMD_IRQ_INTERNAL_ON_ADC_OFF|
                             TOUCH_SCREEN_CMD_RESOLUTION_12BIT;

    printc("[TouchPanel_Init]FT6X36\r\n");
    
    // Interrupt pin. Need to pull high to active.
    // Low   - Touch down
    // Hight - No touch
    pPAD->PAD_IO_CFG_PCGPIO[0x1B] = MMP_GPIO_PULL_HIGH; // CGPIO27 = GPIO59

    pPAD->PAD_IO_CFG_PCGPIO[0x1A] = MMP_GPIO_PULL_HIGH; //CGPIO26 = HIGH
    MMPC_System_WaitMs(50);
    pPAD->PAD_IO_CFG_PCGPIO[0x1A] = 0X22;
    MMPC_System_WaitMs(150);
    pPAD->PAD_IO_CFG_PCGPIO[0x1A] = MMP_GPIO_PULL_HIGH;
    MMPC_System_WaitMs(50);

#if 1
    //for(ubSlaveAddr=0x60;ubSlaveAddr<=0x70;ubSlaveAddr++){
    //    m_TouchPanelI2c.ubSlaveAddr = ubSlaveAddr>>1;
        m_TouchPanelI2c.bRfclModeEn = MMP_FALSE;
        uc_reg_addr = FTS_REG_POINT_RATE;               
        MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c,uc_reg_addr,&uc_reg_value,1);
        printc("mtk_tpd[FTS] report rate is %dHz.\n",uc_reg_value * 10);
    //}

    uc_reg_addr = FTS_REG_FW_VER;
    MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c,uc_reg_addr,&uc_reg_value,1);
    printc("mtk_tpd[FTS] Firmware version = 0x%x\n", uc_reg_value);


    uc_reg_addr = FTS_REG_CHIP_ID;
    MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c,uc_reg_addr,&uc_reg_value,1);
    printc("mtk_tpd[FTS] chip id is %d.\n",uc_reg_value);
#endif

    InitTouchGpio(TOUCH_PANEL_INT_GPIO, (GpioCallBackFunc*) TOUCH_ISR );

#if 0
    MMPC_System_WaitMs(450);
    //m_TouchPanelI2c.bRfclModeEn = MMP_FALSE;
    MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c,0,readtouch,4);

    if ((0x55 != readtouch[0]) || (0x55 !=readtouch[1]) || (0x55 != readtouch[2]) || (0x55 !=readtouch[3]))
	{
		printc("[TouchPanel_Init]HELLO failed, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n",readtouch[0], readtouch[1], readtouch[2], readtouch[3]);
	}
	else
	{
	    printc("HELLO OK\r\n");
	}
#endif

#ifdef FTS_GESTRUE    
        auc_i2c_write_buf[0] = 0xd0;
        auc_i2c_write_buf[1] = 0x01;
        ft6x06_i2c_Write(client, auc_i2c_write_buf, 2); //let fw open gestrue function
    
        auc_i2c_write_buf[0] = 0xd1;
        auc_i2c_write_buf[1] = 0xff;
        ft6x06_i2c_Write(client, auc_i2c_write_buf, 2);
        /*
        auc_i2c_write_buf[0] = 0xd0;
        auc_i2c_write_buf[1] = 0x00;
        ft6x06_i2c_Write(client, auc_i2c_write_buf, 2); //let fw close gestrue function 
        */
#endif

    // Write PD0 = 0, to enable interrupt pin.
	//status = MMPF_I2cm_WriteReg(&m_TouchPanelI2c, NULL, usWriteData);
    
    return status;
}

void TouchPanel_TransformPosition(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir) // Set dir = 0 as lcd normal, 1: reverse.
{
	//#if (VERTICAL_LCD == 0)
	MMP_USHORT tpRefCenterX, tpRefCenterY,usPanelW,usPanelH;

	#if (TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_0)
	if(dir)//LCD reverse
	{
		*pX = FT6X36_ADC_STEPS_X-*pX;
		*pY = FT6X36_ADC_STEPS_Y-*pY;
	}
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_1)
	if(dir)
	{
    	*pX = FT6X36_ADC_STEPS_X-*pX;
	}
    else
    {
        *pY = FT6X36_ADC_STEPS_Y-*pY;
    }
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_2)
	if(dir)
	{
    	*pY = FT6X36_ADC_STEPS_Y-*pY;
	}
    else
    {
        *pX = FT6X36_ADC_STEPS_X-*pX;
    }
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_3)
	if(!dir)//LCD Normal
	{
		*pX = FT6X36_ADC_STEPS_X-*pX;
		*pY = FT6X36_ADC_STEPS_Y-*pY;
	}
	#endif

	//#endif///< (VERTICAL_LCD == 0)

    //MAPPING TO RESOLUTION
    tpRefCenterX = *pX;
    tpRefCenterY = *pY;

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
    usPanelW = RTNA_LCD_GetAttr()->usPanelW;
    usPanelH = RTNA_LCD_GetAttr()->usPanelH;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    usPanelW = RTNA_LCD_GetAttr()->usPanelH;
    usPanelH = RTNA_LCD_GetAttr()->usPanelW;
#endif

    *pX = (tpRefCenterX * usPanelW)/FT6X36_ADC_STEPS_X;
    *pY = (tpRefCenterY * usPanelH)/FT6X36_ADC_STEPS_Y;

}
	
int TouchPanel_ReadPosition(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir)
{
    MMP_USHORT usReadData[4] = {0};
    MMP_USHORT usWriteData = 0;
    MMP_UBYTE  Temp[4];
    MMP_ERR ret= 0;
#if 0
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
#endif

    MMP_USHORT buf[POINT_READ_BUF] = { 0 };
    MMP_LONG i = 0;
    MMP_USHORT pointid = FT_MAX_ID;
    MMP_ERR status = MMP_ERR_NONE;
    MMP_USHORT touch_point = 0;
    
    m_TouchPanelI2c.bRfclModeEn = MMP_FALSE;
    status = MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c,buf[0],&buf[0],POINT_READ_BUF);

    //ret = ft6x06_i2c_Read(data->client, buf, 1, buf, POINT_READ_BUF);
    if (status != MMP_ERR_NONE) {
        printc("TouchPanel_ReadPosition fail\r\n");
        return ret;
    }

	for (i = 0; i < CFG_MAX_TOUCH_POINTS; i++)
	{
		pointid = (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
		if (pointid >= FT_MAX_ID)
			break;
		else
			touch_point++;
        
		*pX =(MMP_USHORT) (buf[FT_TOUCH_X_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
		    8 | (MMP_USHORT) buf[FT_TOUCH_X_L_POS + FT_TOUCH_STEP * i];
		*pY=(MMP_USHORT) (buf[FT_TOUCH_Y_H_POS + FT_TOUCH_STEP * i] & 0x0F) <<
		    8 | (MMP_USHORT) buf[FT_TOUCH_Y_L_POS + FT_TOUCH_STEP * i];
		//event->au8_touch_event[i] =
		//    buf[FT_TOUCH_EVENT_POS + FT_TOUCH_STEP * i] >> 6;
		//event->au8_finger_id[i] =
		//    (buf[FT_TOUCH_ID_POS + FT_TOUCH_STEP * i]) >> 4;
	}
	

    
    //printc("FT6X36(%d point):pX %d  pY %d\r\n",touch_point, *pX,  *pY);
    TouchPanel_TransformPosition(pX,pY,dir);
//	printc("FT6X36(%d point):pX %d  pY %d\r\n",touch_point, *pX,  *pY);
    
    if( touch_point == 0 )
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

    printc("TouchPanel_ResetCtrl : bon %d\r\n",bOn);

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

#endif //TOUCH_PANEL_FT6X36

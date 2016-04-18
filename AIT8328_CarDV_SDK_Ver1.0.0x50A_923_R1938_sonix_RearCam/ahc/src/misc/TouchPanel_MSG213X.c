/*===========================================================================
 * Include files
 *===========================================================================*/


#include "mmph_hif.h"
#include "mmpf_pio.h"
#include "mmpf_i2cm.h"
#include "TouchPanel_MSG213X.h"
#include "lib_retina.h"
#include "UartShell.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#if ( DSC_TOUCH_PANEL == TOUCH_PANEL_MSG213X)

#define TOUCH_PANEL_I2C_SLAVE_ADDRESS 				0x4C >> 1 // for AIT I2C HW setting, slave address 0x90 should shif 1 bit (>> 1)
#define TOUCH_PANEL_I2C_SCL_GPIO                    MMP_GPIO38
#define TOUCH_PANEL_I2C_SDA_GPIO                    MMP_GPIO39
#define TOUCH_PANEL_I2C_IRQ_GPIO                    MMP_GPIO40

#define DELAY_AFTER_SLAVE_ADDR_IN_US    210     // I2C delay time after slave addr in micro second.
#define NUM_OF_TOUCH_PANEL_DATA         8       // size in ushort


#define MSG213X_ADC_STEPS						    2047
//#define TOUCH_PANEL_READ_IN_ISR
static int bTpDebug = 0;
static MMP_ULONG ulLastPosition;

#define TP_STATUS_RELEASED      0
#define TP_STATUS_PRESSED       1

static int nTpStatus = TP_STATUS_RELEASED;
static MMP_ULONG ulLastValidTime =0;
static MMP_ULONG ulLastEventTime  =0;

#define TP_CONTINUE_TIME            20 //30 //600 //Rogers: The TP send more PRESS event. 
#define TP_RELEASE_DETERMINED_TIME  200

extern MMP_ULONG	glAhcBootComplete;
int TouchPanelGetStatus(void)
{
    int nStatus;
    OS_CRITICAL_INIT();

    OS_ENTER_CRITICAL();
    nStatus = nTpStatus;
    OS_EXIT_CRITICAL();
        
    return nStatus;
}

void TouchPanelSetStatus(int nStatus)
{
    OS_CRITICAL_INIT();
    OS_ENTER_CRITICAL();
    nTpStatus = nStatus;
    OS_EXIT_CRITICAL();
}

/*===========================================================================
 * Global varible
 *===========================================================================*/
#if 0
static MMP_I2CM_ATTR  m_TouchPanelI2c = {      MMP_I2CM1,               //MMP_I2CM_ID i2cmID;             //MMP_I2CM0 ~ MMP_I2CM2 stand for HW I2CM                                                                
                                               TOUCH_PANEL_I2C_SLAVE_ADDRESS,             //MMP_UBYTE ubSlaveAddr;                                                                                                                              
                                               0,                   //MMP_UBYTE ubRegLen;              //Indicate register as the 8 bit mode or 16 bit mode.                                                              
                                               8,                   //MMP_UBYTE ubDataLen;             //Indicate data as the 8 bit mode or 16 bit mode.                                                                  
                                               0x20,                //MMP_UBYTE ubDelayTime;           //The delay time after each ACK/NACK, which is used in SW I2CM (i2cmID = MMP_I2CM_GPIO or MMP_I2CM_SNR)
                                               MMP_FALSE,           //MMP_BOOL  bDelayWaitEn;          //HW feature, to set delay between each I2CM accessing set                                                         
                                               MMP_FALSE,           //MMP_BOOL  bInputFilterEn;        //HW feature, to filter input noise                                                                                
                                               MMP_FALSE,           //MMP_BOOL  b10BitModeEn;          //HW I2CM supports 10 bit slave address, the bit8 and bit9 are in ubSlaveAddr1                                     
                                               MMP_FALSE,           //MMP_BOOL  bClkStretchEn;         //HW support stretch clock                                                                                         
                                               0x0,                 //MMP_UBYTE ubSlaveAddr1;          //10 bit slave address support used.                                                                               
                                               0x0,                 //MMP_UBYTE ubDelayCycle;          //When bDelayWaitEn enable, set the delay cycle after each 8 bit transmission                                      
                                                //3
                                               0x5,                 //MMP_UBYTE ubPadNum;              //HW pad map, the relate pad definition, please refer global register spec.                                        
                                               100000 /*100KHZ*/,   //MMP_ULONG ulI2cmSpeed;           //SW and HW I2CM speed control, the unit is HZ.                                                                    
                                               MMP_TRUE,            //MMP_BOOL  bOsProtectEn;          //Enable: I2CM driver with OS semaphore protect.                                                                   
                                               NULL,                //MMP_GPIO_PIN sw_clk_pin;         //Used in SW I2CM (i2cmID = MMP_I2CM_GPIO only), indicate the clock pin                                          
                                               NULL,                //MMP_GPIO_PIN sw_data_pin;        //Used in SW I2CM (i2cmID = MMP_I2CM_GPIO only), indicate the data pin                                           
                                               MMP_TRUE,            //MMP_BOOL  bRfclModeEn;           //Used in read from current location mode. Read data without send register address.                                
                                               MMP_TRUE,            //MMP_BOOL  bWfclModeEn;           //Used in write from current location mode. Write data without send register address.                              
                                               MMP_FALSE,
                                               0};                     
#else
static MMP_I2CM_ATTR  m_TouchPanelI2c = {      I2CM_GPIO,           //MMP_I2CM_ID i2cmID;             //MMP_I2CM0 ~ MMP_I2CM3 stand for HW I2CM                                                                
                                               0x4C,                //MMP_UBYTE ubSlaveAddr;                                                                                                                              
                                               0,                   //MMP_UBYTE ubRegLen;              //Indicate register as the 8 bit mode or 16 bit mode.                                                              
                                               8,                   //MMP_UBYTE ubDataLen;             //Indicate data as the 8 bit mode or 16 bit mode.                                                                  
                                               10,                  //MMP_UBYTE ubDelayTime;           //The delay time after each ACK/NACK, which is used in SW I2CM (i2cmID = MMP_I2CM_GPIO or MMP_I2CM_SNR)
                                               MMP_FALSE,           //MMP_BOOL  bDelayWaitEn;          //HW feature, to set delay between each I2CM accessing set                                                         
                                               MMP_FALSE,           //MMP_BOOL  bInputFilterEn;        //HW feature, to filter input noise                                                                                
                                               MMP_FALSE,           //MMP_BOOL  b10BitModeEn;          //HW I2CM supports 10 bit slave address, the bit8 and bit9 are in ubSlaveAddr1                                     
                                               MMP_FALSE,           //MMP_BOOL  bClkStretchEn;         //HW support stretch clock                                                                                         
                                               0x0,                 //MMP_UBYTE ubSlaveAddr1;          //10 bit slave address support used.                                                                               
                                               0x0,                 //MMP_UBYTE ubDelayCycle;          //When bDelayWaitEn enable, set the delay cycle after each 8 bit transmission                                      
                                               0x3,                 //MMP_UBYTE ubPadNum;              //HW pad map, the relate pad definition, please refer global register spec.                                        
                                               100000 /*100KHZ*/,   //MMP_ULONG ulI2cmSpeed;           //SW and HW I2CM speed control, the unit is HZ.                                                                    
                                               MMP_FALSE,            //MMP_BOOL  bOsProtectEn;          //Enable: I2CM driver with OS semaphore protect.                                                                   
                                               TOUCH_PANEL_I2C_SCL_GPIO, //MMP_GPIO_PIN sw_clk_pin;         //Used in SW I2CM (i2cmID = MMP_I2CM_GPIO only), indicate the clock pin                                          
                                               TOUCH_PANEL_I2C_SDA_GPIO, //MMP_GPIO_PIN sw_data_pin;        //Used in SW I2CM (i2cmID = MMP_I2CM_GPIO only), indicate the data pin                                           
                                               MMP_TRUE,            //MMP_BOOL  bRfclModeEn;           //Used in read from current location mode. Read data without send register address.                                
                                               MMP_TRUE,            //MMP_BOOL  bWfclModeEn; 
											   MMP_FALSE,
											   0};
#endif


/*===========================================================================
 * Main body
 *===========================================================================*/

typedef struct tagTouchPoint
{
	MMP_USHORT x;
	MMP_USHORT y;
} TPoint;

static BYTE Calculate_8BitsChecksum( MMP_USHORT *msg, int s32Length )
{
	int s32Checksum = 0;
	int i;

	for ( i = 0 ; i < s32Length; i++ )
	{
		s32Checksum += msg[i];
	}
	return (BYTE)( ( -s32Checksum ) & 0xFF );
}

static MMP_BOOL TouchPanelGetPos( MMP_USHORT Buf[8], TPoint* pPt1, TPoint* pPt2 )
{

	TPoint pt1={0},pt2={0};
	int Fingers=0;
	if(Buf[0]==0x52 &&Buf[7] == Calculate_8BitsChecksum( Buf, 7))
	{
		pt1.x=((Buf[1] & 0xF0)<<4)|Buf[2];
		pt1.y=((Buf[1] & 0x0F)<<8)|Buf[3];
		pt2.x=((Buf[4] & 0xF0)<<4)|Buf[5];
		pt2.y=((Buf[4] & 0x0F)<<8)|Buf[6];

		if((pt2.x) & 0x0800)
		{
			pt2.x |= 0xF000;
		}
		if((pt2.y) & 0x0800)
		{
			pt2.y |= 0xF000;
		}

		pt2.x+=pt1.x;
		pt2.y+=pt1.y;

		if(Buf[1]== 0xFF&& Buf[2]== 0xFF&& Buf[3]== 0xFF)
		{
			Fingers = 0;//Touch End
            return MMP_FALSE;			
		}
		else if((pt1.x == pt2.x) && (pt1.y == pt2.y))
		{
            pt2.x = 0;
            pt2.y = 0;
			Fingers = 1;
		}
		else
		{
			Fingers = 2;
		}
        TouchPanelTransformPosition((MMP_USHORT *)&pt1.x, (MMP_USHORT *)&pt1.y);
        *pPt1 = pt1;
        *pPt2 = pt2;

        if( bTpDebug)
        {
            printc( "ctp_ %d=(%d,%d),(%d,%d) \n", Fingers, pt1.x, pt1.y, pt2.x, pt2.y );
        }

        return MMP_TRUE;        

	}
    else if( bTpDebug)
    {
        printc( "ctp %d=(%d,%d),(%d,%d) \n", Fingers, pt1.x, pt1.y, pt2.x, pt2.y );
        return MMP_FALSE;
    }

    //No legal value
	return MMP_FALSE;
}


static 
void TOUCH_ISR(MMP_GPIO_PIN piopin)
{

    MMP_ULONG  ulNow;

#ifdef TOUCH_PANEL_READ_IN_ISR
	{
	    MMP_UBYTE  tempValue = 0;
	    TPoint pt1, pt2;
	    MMPF_PIO_GetData( piopin, &tempValue);

	    MMP_USHORT usReadData[8]={0};

	    MMPF_OS_GetTime(&ulNow); 
	    MMPF_I2cm_ReadBurstDataWithDelay(&m_TouchPanelI2c, 
	                                        NULL, 
	                                        usReadData, 
	                                        NUM_OF_TOUCH_PANEL_DATA, 
	                                        DELAY_AFTER_SLAVE_ADDR_IN_US);
	    if( bTpDebug)
	    {
	        MemoryDump((char*)usReadData, 0x16);
	    }


	    TouchPanelGetPos(usReadData, &pt1, &pt2 );
	}
#else

    MMPF_OS_GetTime(&ulNow);
	
	if(glAhcBootComplete == MMP_TRUE)
    SetKeyPadEvent(TOUCH_S14_PRESS);
#endif
}

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

void TouchPanel_Reset(void)
{

    printc("TouchPanel_Reset\n");

    if(TOUCH_RESET_GPIO != 0xFFF)
    {
        MMPF_PIO_EnableOutputMode(TOUCH_RESET_GPIO, MMP_TRUE, MMP_TRUE );        

        MMPF_PIO_SetData(TOUCH_RESET_GPIO, TOUCH_RESET_GPIO_ACT_MODE, MMP_TRUE);
    }

    RTNA_WAIT_MS(100);

}
	

int TouchPanel_Init(void)
{

    printc("TouchPanel_Init\n");

    TouchPanel_Reset();
	
    InitTouchGpio(TOUCH_PANEL_I2C_IRQ_GPIO, (GpioCallBackFunc*) TOUCH_ISR );

    MMPF_PIO_EnableOutputMode(TOUCH_PANEL_I2C_SCL_GPIO, MMP_FALSE, MMP_TRUE );
    MMPF_PIO_EnableOutputMode(TOUCH_PANEL_I2C_SDA_GPIO, MMP_FALSE, MMP_TRUE );

    // Write PD0 = 0, to enable interrupt pin.

    return 1;
}
	

int TouchPanelCheckUpdate(MMP_ULONG* pulKeyEvent, MMP_ULONG* pulPosition, MMP_UBYTE dir)
{
    *pulPosition = 0;

    if( *pulKeyEvent == TOUCH_S14_PRESS   ||
        *pulKeyEvent == TOUCH_S14_MOVE       )
    {
        TPoint pt1, pt2;
        MMP_USHORT usReadData[8]={0};
        MMP_ULONG  ulNow;
        int        nStatus = TouchPanelGetStatus();

        MMPF_OS_GetTime(&ulNow);

        MMPF_I2cm_ReadBurstDataWithDelay(&m_TouchPanelI2c, 
                                            NULL, 
                                            usReadData, 
                                            NUM_OF_TOUCH_PANEL_DATA, 
                                            DELAY_AFTER_SLAVE_ADDR_IN_US);
        if( bTpDebug)
        {
            MemoryDump((char*)usReadData, 0x16);
        }
        
        if( TouchPanelGetPos(usReadData, &pt1, &pt2 ))
        {           
            *pulPosition = pt1.x|(pt1.y<<16);
            nStatus =  TouchPanelGetStatus();
            ulLastPosition =  *pulPosition;
            ulLastValidTime = ulNow;

            if(nStatus == TP_STATUS_RELEASED ) //not pressed
            {
                TouchPanelSetStatus(TP_STATUS_PRESSED);
                //printc("tp pressed %d %d\n", pt1.x, pt1.y);
                ulLastEventTime = ulNow;
            }
            else
            {
                // If now is pressed
                if( ulNow - ulLastEventTime > TP_CONTINUE_TIME )
                {
                    // continue press
                    ulLastEventTime = ulNow;
                    *pulKeyEvent = TOUCH_S14_MOVE;
                    //printc("tp move %d %d\n", pt1.x, pt1.y);
                    return 1;
                }
                else
                {
                    return 0;
                }
            }

            return 1;
        }
        else
        {
            ulLastPosition = 0;
            return 0;
        }

    }
    else if (*pulKeyEvent == TOUCH_S14_RELEASE )
    {
        *pulPosition = ulLastPosition;
    }

    return 1;
}

int TouchPanelCheckRelease(void)
{
    int        nStatus = TouchPanelGetStatus();
    MMP_ULONG  ulNow;
    
    MMPF_OS_GetTime(&ulNow);

    if( ulLastValidTime != 0 &&
        ulNow - ulLastValidTime > TP_RELEASE_DETERMINED_TIME )
    {
        if( nStatus == TP_STATUS_PRESSED ) //not pressed
        {
            TouchPanelSetStatus(TP_STATUS_RELEASED);            
            ulLastValidTime = 0;
			//TouchPanel_Reset(); //Rogers: Sometime the TP doesn't respont.
            return 1;
        }
    }
    return 0;
}


int TouchPanelReadVBAT1( MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir )
{
	return 1;
}

void TouchPanelTransformPosition(MMP_USHORT* pX, MMP_USHORT* pY)
{
	
	MMP_USHORT tpRefCenterX = 1024, tpRefCenterY = 1024, tpStepX = 6, tpStepY = 8;
	MMP_USHORT usTemp;
	#if (TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_0)
	
	{
		usTemp = *pY;
		*pX = MSG213X_ADC_STEPS-*pX;
		*pY = *pX;
		*pX = usTemp;  
	}
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_1)
	//TCD
	
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_2)
	//TCD
	
	#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_3)
	//TCD
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
	
	
	
}

void TouchPanelGetPosition(MMP_USHORT *x, MMP_USHORT *y)
{
	*x = (MMP_USHORT)(ulLastPosition & 0xFFFF);  
	*y = (MMP_USHORT)((ulLastPosition >> 16) & 0xFFFF);
}
#endif //TOUCH_PANEL_MSG213X

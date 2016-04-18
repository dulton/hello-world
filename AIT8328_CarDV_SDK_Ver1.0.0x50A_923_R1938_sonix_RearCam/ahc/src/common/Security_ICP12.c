#define PASSIC
//
#ifdef PASSIC
#undef  PASSIC
//==============================================================================
//
//  File        : PassIc.c
//  Description : Check security IC. It is use "P12" Protect-IC
//  Author      : Karl Chen
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include file
 *===========================================================================*/ 

#include "includes_fw.h"
#include "mmpf_typedef.h"
#include "os_wrap.h"
#include "mmpd_display.h"
#include "mmps_fs.h"
#include "mmpf_fs_api.h"
#include "mmps_vidplay.h"
#include "mmpf_uart.h"
#include "mmpf_hif.h"
#include "mmpf_system.h"
#include "mmpf_display.h"
#include "ait_config.h"
#include "mmp_reg_gpio.h"
#include "mmpf_pio.h"
#include "controlIC_adx2002.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define GPIONUM_ICSCL		MMP_GPIO35
#define GPIONUM_ICSDA		MMP_GPIO34
#define GPIONUM_ICCE		MMP_GPIO36
#define I2C_ACK				0x00
#define I2C_NACK			0x01
#define I2C_SlaveACK		0x02
#define I2C_W				0x00
#define I2C_R				0x01
#define MODE_ADDR			0x00
#define MODE_DATA			0x01
#define	ADDRESS				0x64//8bit address
#define PASS				0
#define ERR 				1

const unsigned char VProtect[] = {//64 BYTE INDEX 0~0X3F : 1~0X40 
0x03,0x13,0x02,0x12,0x10,0x08,0x05,0x15,0x06,0x1B,0x09,0x04,0x0B,0x1A,0x0D,0x0F,
0x11,0x01,0x0E,0x1C,0x16,0x20,0x17,0x1E,0x18,0x07,0x19,0x0A,0x1D,0x0C,0x1F,0x14,

0x3F,0x21,0x3A,0x28,0x40,0x2A,0x3E,0x38,0x2D,0x3B,0x2E,0x3C,0x30,0x26,0x32,0x2F,
0x33,0x24,0x35,0x27,0x36,0x37,0x23,0x25,0x31,0x39,0x29,0x22,0x3D,0x2B,0x34,0x2C
}; 

const unsigned char VID[] = {//4 BYTE ID
0X20,	
0X11,
0X10,
0X17
};


/*===========================================================================
 * Function Prototype
 *===========================================================================*/ 

MMP_UBYTE	PassIc_Detect(MMP_UBYTE index); 
void 		I2CReset(void);
void 		I2CStart(void);
void	 	I2CWrite(MMP_BOOL ad, MMP_UBYTE data, MMP_BOOL rw);
void	 	I2CACK(MMP_UBYTE ack);
void 		I2CStop(void);
void 		I2CTransWait(int count);
MMP_ERR 	ADX2003_PowerOff(void);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

//------------------------------------------------------------------------------
//  Function    : I2CReset
//  Description : De-assert IC first and then assert IC
//------------------------------------------------------------------------------
/**
    @brief  The function will de-assert CE# first and then assert CE#
    @param[in] 
    @return 
*/
 void I2CReset(void)
{
	RTNA_Wait_Count(5);//6us
	
	MMPF_PIO_SetData(GPIONUM_ICCE, GPIO_HIGH, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICCE, MMP_TRUE, MMP_TRUE);	
	RTNA_Wait_Count(80000);
		
	MMPF_PIO_SetData(GPIONUM_ICCE, GPIO_LOW, MMP_TRUE);	
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_HIGH, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_TRUE, MMP_TRUE);
	MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_HIGH, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_TRUE, MMP_TRUE);
	RTNA_Wait_Count(4000);	
}

//------------------------------------------------------------------------------
//  Function    : I2CStart
//  Description : Initiate "START" of I2C protocol
//------------------------------------------------------------------------------
/**
    @brief  Initiate "START" of I2C protocol
    @param[in] 
    @return 
*/
void I2CStart(void)
{
	RTNA_Wait_Count(5);//6us
		
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_HIGH, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_TRUE, MMP_TRUE);
	MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_HIGH, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_TRUE, MMP_TRUE);
	RTNA_Wait_Count(5);//2.4us
	
	MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_LOW, MMP_TRUE);	
	RTNA_Wait_Count(100);
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
	RTNA_Wait_Count(10);
}

//------------------------------------------------------------------------------
//  Function    : I2CWrite
//  Description : Initiate write transaction of I2C protocol.
//				  It only write one byte at one time.
//				  It calls I2CACK to generates ack in the end of transaction
//------------------------------------------------------------------------------
/**
    @brief  Initiate write transaction of I2C protocol.
    @param[in] ad	:	write address or write data
	@param[in] data	:	value which you want writing to chip
	@param[in] rw	:	read or write. Address of read is different from write
    @return 
*/

void I2CWrite(MMP_BOOL ad, MMP_UBYTE data, MMP_BOOL rw)
{
	MMP_UBYTE	outByte;
	MMP_BOOL	outData;
	MMP_UBYTE	i;
	
	if(ad==MODE_ADDR)//Address phase
		outByte=ADDRESS|rw;
	else if(ad==MODE_DATA)//data phase
		outByte=data;
	
	for(i=0;i<8;i++)
	{
		outData = ( outByte & (0x80>>i) ) >> (7-i);
		
		if(outData==0)
		{
			MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_LOW, MMP_TRUE);
			
		}
		else if(outData==1)
		{
			MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_HIGH, MMP_TRUE);
		}
		
		MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_TRUE, MMP_TRUE);
		RTNA_Wait_Count(100);//2.4us	
		MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_HIGH, MMP_TRUE);
		MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_TRUE, MMP_TRUE);
		RTNA_Wait_Count(100);//2.4us
		MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
		RTNA_Wait_Count(100);//2.4us		
	}
	
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_FALSE, MMP_TRUE);
	I2CACK(I2C_SlaveACK);	
}

//------------------------------------------------------------------------------
//  Function    : I2CRead
//  Description : Initiate read transaction of I2C protocol.
//				  It only read one byte at one time.
//------------------------------------------------------------------------------
/**
    @brief  Initiate read transaction of I2C protocol.
    @param[in] 
    @return		It returns data reading from I2C chip 
*/
MMP_UBYTE I2CRead(void)
{
	MMP_UBYTE	inByte;
	MMP_BOOL	inData;
	MMP_UBYTE	i;
	
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_FALSE, MMP_TRUE);	
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_TRUE, MMP_TRUE);
	RTNA_Wait_Count(100);//2.4us	
	
	inByte=0x00;
	inData=0x00;
	
	for(i=0;i<8;i++)
	{
		MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_HIGH, MMP_TRUE);
		RTNA_Wait_Count(100);//2.4us
		
		MMPF_PIO_GetData(GPIONUM_ICSDA,&inData);
		inByte|=inData;
		
		if(i<7)
			inByte=inByte<<1;
		
		RTNA_Wait_Count(100);//2.4us
		MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
		RTNA_Wait_Count(100);//2.4us
	}
	return inByte;
}

//------------------------------------------------------------------------------
//  Function    : I2CACK
//  Description : Initiate ACK of I2C protocol.
//				  It can generate ACK or Non-Ack.
//				  It also can switch pin to input mode when it needs ack from master-write transaction.
//				  However, we disable judging value of ack for timing issue.
//------------------------------------------------------------------------------
/**
    @brief  Initiate ACK of I2C protocol.
    @param[in]	ack: generate ack or non-ack. Or switch to input mode for master-write transaction
    @return		
*/
void I2CACK(MMP_UBYTE ack)
{
	MMP_UBYTE tempValue;
	tempValue=1;

	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_TRUE, MMP_TRUE);
	
	if(ack==I2C_ACK)
	{
		MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_LOW, MMP_TRUE);
		MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_TRUE, MMP_TRUE);
	}
	else if(ack==I2C_NACK)
	{
		MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_HIGH, MMP_TRUE);
		MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_TRUE, MMP_TRUE);
	}
	else if(ack==I2C_SlaveACK)
	{
		MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_FALSE, MMP_TRUE);
	}
	
	RTNA_Wait_Count(4);//2.4us
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_HIGH, MMP_TRUE);
	RTNA_Wait_Count(400);
	if(ack==I2C_SlaveACK)
	{
		MMPF_PIO_GetData(GPIONUM_ICSDA,&tempValue);
	}
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
	RTNA_Wait_Count(2);//2.4us
}

//------------------------------------------------------------------------------
//  Function    : I2CStop
//  Description : Initiate "STOP" of I2C protocol.			  
//------------------------------------------------------------------------------
/**
    @brief  Initiate "STOP" of I2C protocol.
    @param[in]
    @return		
*/
void I2CStop(void)
{
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_LOW, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_TRUE, MMP_TRUE);
	MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_LOW, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_TRUE, MMP_TRUE);
	RTNA_Wait_Count(2);//2.4us
	MMPF_PIO_SetData(GPIONUM_ICSCL, GPIO_HIGH, MMP_TRUE);
	RTNA_Wait_Count(4);
	MMPF_PIO_SetData(GPIONUM_ICSDA, GPIO_HIGH, MMP_TRUE);
	RTNA_Wait_Count(100);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSCL, MMP_FALSE, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(GPIONUM_ICSDA, MMP_FALSE, MMP_TRUE);
}

void I2CTransWait(int count)
{
	RTNA_Wait_Count(count);
}

//------------------------------------------------------------------------------
//  Function    : PassIc_Detect
//  Description : It communicates with protect IC with chip "P12"
//				  It also check value from protect IC with algorithm providing by "P12"
//------------------------------------------------------------------------------
/**
    @brief  Communicates and check with protect IC
    @param[in]	index	:	write to protect IC to generate 6 bytes data for checking.
    @return		It returns PASS or ERROR which is checking result.
*/
MMP_UBYTE PassIc_Detect(MMP_UBYTE index)
{
	MMP_UBYTE data[6]={0};
	MMP_UBYTE byteCount=0;
	MMP_UBYTE i,get_val;
	MMP_UBYTE DI[8],QI[8],mask;
	
	//I2C Read/Write#

	I2CReset();
	I2CStart();
	I2CWrite(MODE_ADDR, 0, I2C_W);
	I2CTransWait(1000);
	I2CWrite(MODE_DATA, 0x01, I2C_W);
	I2CTransWait(1000);
	I2CWrite(MODE_DATA, index, I2C_W);
	I2CStop();
	
	I2CTransWait(1500000);
	I2CStart();
	I2CWrite(MODE_ADDR, 0, I2C_R);
	
	for(byteCount=0; byteCount<6; byteCount++)
	{
		I2CTransWait(1000);
		data[byteCount]=I2CRead();
		if(byteCount<5)
			I2CACK(I2C_ACK);
		else
			I2CACK(I2C_NACK);
	}
	I2CStop();

	for(i=0;i<6;i++)
	{
		RTNA_DBG_Str(0,"ProtectIC data[");RTNA_DBG_Long(0, i);RTNA_DBG_Str(0,"]===========");
		RTNA_DBG_Long(0, data[i]);RTNA_DBG_Str(0,"\r\n"); 
	}
	
	//TIMER SEED fill to QI as bit buffer
	mask = 1;
	for(i=0;i<8;i++)
	{
		if((data[5] & mask) != 0)		
		{
    		QI[i] = 1;				//use as bit
    	}
    	else
    	{
    		QI[i] = 0;
    	}
    	mask = mask << 1;
	}			

	//Start Logical Rotate
	for (i=0; i < VProtect[index]; i++)			//index tabel
    {
		DI[0] = QI[7];
        DI[1] = QI[0];
		DI[2] = ((QI[1] ^ QI[7]) & 0X01);
        DI[3] = ((QI[2] ^ QI[7]) & 0X01);
        DI[4] = ((QI[3] ^ QI[7]) & 0X01);
        DI[5] = QI[4];
        DI[6] = QI[5];
        DI[7] = QI[6];

		QI[0] = DI[0];
        QI[1] = DI[1];
		QI[2] = DI[2];
        QI[3] = DI[3];
        QI[4] = DI[4];
        QI[5] = DI[5];
        QI[6] = DI[6];
        QI[7] = DI[7];
    }

	//QI[] => get_val as Byte    			
	for(i=0;i<8;i++)
	{
		get_val = get_val >> 1;
		if(QI[i] != 0) get_val |= 0x80;
		else get_val &= 0x7f;
	}

	if(get_val != data[4]) 	//Local val not same as return val, Error
	{
		return ERR;
	}

	//compare ID
	for(i=0;i<4;i++)
	{
		if(VID[i] != data[i]) 
		{
			return ERR;
		}
	}
	
	return PASS;			
}

#endif


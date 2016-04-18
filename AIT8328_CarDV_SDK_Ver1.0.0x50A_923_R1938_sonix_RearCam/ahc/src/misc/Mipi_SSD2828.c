/*********************************************************************
;* Project Name : s3c2443x
;*
;* Copyright 2006 by Samsung Electronics, Inc.
;* All rights reserved.
;*
;* Project Description :
;* This software is only for verifying functions of the s3c2443x
;* Anybody can use this code without our permission.
;**********************************************************************/
#include "Customer_config.h" // CarDV
#include "mmpf_pio.h"
#include "Mipi_SSD2828.h"

#if (ENABLE_MIPI_PANEL)

#if (Lcd_95_A00_540_960)
#define LCD_XSIZE_TFT  (540)
#define LCD_YSIZE_TFT  (960)	

#define LCD_VBPD		(12)   // 13
#define LCD_VFPD		(7)	   // 12
#define LCD_VSPW		(1)	   // 1
#define LCD_HBPD		(13)   // 13
#define LCD_HFPD		(11)   // 11
#define LCD_HSPW		(4)	   // 4

#elif (Lcd_765_E00_480_854)
#define LCD_XSIZE_TFT  (480)
#define LCD_YSIZE_TFT  (854)	

#define LCD_VBPD		(14)
#define LCD_VFPD		(6)
#define LCD_VSPW		(2)
#define LCD_HBPD		(8)
#define LCD_HFPD		(8)
#define LCD_HSPW		(8)

#else
"error";
#endif


#define LSCE_GPIO_PIN   (MMP_GPIO124)
#define LSCK_GPIO_PIN   (MMP_GPIO122)
#define LSDA_GPIO_PIN   (MMP_GPIO123)
#define LSDR_GPIO_PIN   (MMP_GPIO60)


#define Delay_us(n)   MMPC_System_WaitUs(n*100/27)	//(lcm_util.udelay(n))
#define Delay_ms(n)   MMPC_System_WaitMs(n*100/27)	//(lcm_util.mdelay(n))

void Set_RST(unsigned long index)
{
	if(index) MMPF_PIO_SetData(LSDR_GPIO_PIN, GPIO_HIGH, MMP_TRUE);//rGPEDAT |= 0x8000;
	else      MMPF_PIO_SetData(LSDR_GPIO_PIN, GPIO_LOW, MMP_TRUE);//rGPEDAT &= ~0x8000;	
}

void Set_2805_CS(unsigned long index)
{
	if(index) MMPF_PIO_SetData(LSCE_GPIO_PIN, GPIO_HIGH, MMP_TRUE);//rGPEDAT |= 0x8000;
	else       MMPF_PIO_SetData(LSCE_GPIO_PIN, GPIO_LOW, MMP_TRUE);//rGPEDAT &= ~0x8000;	
}

void Set_SCL(unsigned long index)
{
	if(index) MMPF_PIO_SetData(LSCK_GPIO_PIN, GPIO_HIGH, MMP_TRUE);//rGPEDAT |= 0x2000;
	else       MMPF_PIO_SetData(LSCK_GPIO_PIN, GPIO_LOW, MMP_TRUE);//rGPEDAT &= ~0x2000;	
}

void Set_SDI(unsigned long index)
{
	if(index) MMPF_PIO_SetData(LSDA_GPIO_PIN, GPIO_HIGH, MMP_TRUE);//rGPEDAT |= 0x1000;
	else      MMPF_PIO_SetData(LSDA_GPIO_PIN, GPIO_LOW, MMP_TRUE);//rGPEDAT &= ~0x1000;	
}


void SPI_Init(void)
{
	
	//Delay_ms(100);
	
	MMPF_PIO_EnableOutputMode(LSCE_GPIO_PIN, MMP_TRUE, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(LSCK_GPIO_PIN, MMP_TRUE, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(LSDA_GPIO_PIN, MMP_TRUE, MMP_TRUE);
	MMPF_PIO_EnableOutputMode(LSDR_GPIO_PIN, MMP_TRUE, MMP_TRUE);
	//Delay_ms(10);
	Set_2805_CS(1);
	Set_RST(1);
	//Set_CSX(1);
	Set_SCL(0);	
	Set_SDI(0);
	
}

void SPI_3W_SET_Cmd(unsigned char cmd)
{
	unsigned long kk;
	
	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Set_SCL(1);
	

	Set_SCL(0);
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Set_SCL(0);
		cmd = cmd<<1;	
	}
}

void SPI_3W_SET_PAs(unsigned char value)
{
	unsigned long kk;

	Set_SDI(1);			//Set DC=1, for writing to Data register
	Set_SCL(0);
	Set_SCL(1);
	
	Set_SCL(0);
	for(kk=0;kk<8;kk++)
	{
		if((value&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Set_SCL(0);
		value = value<<1;	
	}	
}
//-----------------------------------------------------
void Write_com(unsigned short vv)
{
	Set_2805_CS(0);
	SPI_3W_SET_Cmd(vv&0xff);	
}

void Write_register(unsigned short vv)
{
	SPI_3W_SET_PAs(vv&0xff);
	SPI_3W_SET_PAs((vv>>8)&0xff);	
}
//-----------------------------------------------------

void SPI_2825_WrCmd(unsigned char cmd)
{
	unsigned long kk;
	
	Set_2805_CS(0);
	
	Set_SDI(0);			//Set DC=0, for writing to Command register
	Set_SCL(0);
	Set_SCL(1);
	

	Set_SCL(0);
	for(kk=0;kk<8;kk++)
	{
		if((cmd&0x80)==0x80) Set_SDI(1);
		else         Set_SDI(0);
		Set_SCL(1);
		Set_SCL(0);
		cmd = cmd<<1;	
	}
	
	Set_2805_CS(1);	
}

//void SPI_2825_WrCmd(unsigned char)
void SPI_2825_WrReg(unsigned char c,unsigned short value)
{
	Set_2805_CS(0);
	SPI_3W_SET_Cmd(c);
	SPI_3W_SET_PAs(value&0xff);
	SPI_3W_SET_PAs((value>>8)&0xff);	
	Set_2805_CS(1);	
}

void GP_COMMAD_PA(unsigned short num)
{
	SPI_2825_WrReg(0xbc, num);
	Write_com(0x00bf);
	Set_2805_CS(1);	
}

void SPI_WriteData(unsigned char value)
{
	Set_2805_CS(0);
	SPI_3W_SET_PAs(value);
	Set_2805_CS(1);	
}


#if (Lcd_95_A00_540_960)
void Init_SSD2805_SPI(void)
{
	
 	SPI_Init();
	Delay_ms(10);
	Set_RST(0);// ( rGPFDAT &= (~(1<<3))) ;
	Delay_ms(50);
	Set_RST(1);//  ( rGPFDAT |= (1<<3) ) ;
	Delay_ms(10);

	
	//void SPI_2825_WrCmd(U8)
	//void SPI_2825_WrReg(U8 c,U16 value)
	//Initial code 1: SSD2825 initialization
	//RGB interface configuration 
	SPI_2825_WrReg(0xb1,(LCD_HSPW<<8)|LCD_VSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(LCD_HBPD<<8)|LCD_VBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(LCD_HFPD<<8)|LCD_VFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	//SPI_2825_WrReg(0xb6, 0x2007);				//Video mode and video pixel format 888
	SPI_2825_WrReg(0xb6, 0x0004);				//Video mode and video pixel format       565	
	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode 
	SPI_2825_WrReg(0xde, 0x0001);				//MIPI lane select 
	SPI_2825_WrReg(0xDD, (LCD_HFPD<<8)|LCD_VFPD);
	//SPI_2825_WrReg(0xd6, 0x0004);				//Color order and endianess	BGR
	SPI_2825_WrReg(0xd6, 0x0005);				//Color order and endianess	RGB
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	//Communicate with LCD driver through MIPI 
	SPI_2825_WrReg(0xb7, 0x0342);				//DCS mode 0342
	SPI_2825_WrReg(0xb8, 0x0000);				//VC registe
	SPI_2825_WrReg(0xbc, 0x0000);				//Packet size 
	SPI_2825_WrCmd(0x11);					//LCD driver exit sleep mode
	Delay_ms(100);	
	SPI_2825_WrCmd(0x29);					//Set LCD driver display on 
//	PLL configuration 
	//SPI_2825_WrReg(0xba, 0x8334);				//PLL setting,8028 0x34--52*24/3=416M
	SPI_2825_WrReg(0xba, 0x8322);				//PLL setting,8028    0x27--39*24/3=312M
	
	SPI_2825_WrReg(0xbb, 0x0006);				//LP clock divider
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
//	SPI_2825_WrReg(0xb7, 0x030B);				//Generic mode, HS video mode
	
	Delay_ms(55);
	
	//Initial code 2: LCD driver initialization
	//MIPI lane and PLL configuration
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb7, 0x0150);				//Generic mode, LP mode
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register
	//Send command and data through MIPI 
	//------------------------------------------------------------------------------------------	
	//----------communicate with lcd driver through mipi----------------------------------------
	
	GP_COMMAD_PA(4);
	SPI_WriteData(0xB9);
	SPI_WriteData(0xFF);
	SPI_WriteData(0x83);
	SPI_WriteData(0x89);


	GP_COMMAD_PA(21);
	SPI_WriteData(0xB1);
	SPI_WriteData(0x64);
	SPI_WriteData(0x10);
	SPI_WriteData(0x10);
	SPI_WriteData(0x2E);
	SPI_WriteData(0x2E);
	SPI_WriteData(0x50);
	SPI_WriteData(0xF0);
	SPI_WriteData(0x5C);
	SPI_WriteData(0x5C);
	SPI_WriteData(0x80);
	SPI_WriteData(0xB8);
	SPI_WriteData(0xB8);
	SPI_WriteData(0xF8);
	SPI_WriteData(0x22);//22  02  12  11
	SPI_WriteData(0x22);
	SPI_WriteData(0x22);

	SPI_WriteData(0x00);
	SPI_WriteData(0x80);
	SPI_WriteData(0x30);
	SPI_WriteData(0x00);

	GP_COMMAD_PA(11);
	SPI_WriteData(0xB2);
	SPI_WriteData(0x80);  //80
	SPI_WriteData(0x50);
	SPI_WriteData(0x0D);
	SPI_WriteData(0x07);
	SPI_WriteData(0x00);
	SPI_WriteData(0x38);
	SPI_WriteData(0x11);
	SPI_WriteData(0x64);
	SPI_WriteData(0x5D);
	SPI_WriteData(0x09);

	GP_COMMAD_PA(14);
	SPI_WriteData(0xB4);
	SPI_WriteData(0x08);//10
	SPI_WriteData(0x30);//66
	SPI_WriteData(0x08);//01
	SPI_WriteData(0x28);//66
	SPI_WriteData(0x05);//01
	SPI_WriteData(0x36);//66
	SPI_WriteData(0x04);//11
	SPI_WriteData(0x58);//78
	SPI_WriteData(0x08);//26
	SPI_WriteData(0x58);//71
	SPI_WriteData(0xB0);
	SPI_WriteData(0x00);
	SPI_WriteData(0xFF);

	GP_COMMAD_PA(38);
	SPI_WriteData(0xD3);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x08);
	SPI_WriteData(0x00);
	SPI_WriteData(0x32);
	SPI_WriteData(0x10);
	SPI_WriteData(0x04);
	SPI_WriteData(0x00);
	SPI_WriteData(0x04);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x37);
	SPI_WriteData(0x33);
	SPI_WriteData(0x08);
	SPI_WriteData(0x08);
	SPI_WriteData(0x37);
	SPI_WriteData(0x08);
	SPI_WriteData(0x08);
	SPI_WriteData(0x37);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x00);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);


	GP_COMMAD_PA(42);
	SPI_WriteData(0xD5);
	SPI_WriteData(0x19);
	SPI_WriteData(0x19);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x19);
	SPI_WriteData(0x19);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x02);
	SPI_WriteData(0x03);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x00);
	SPI_WriteData(0x01);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x06);
	SPI_WriteData(0x07);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x04);
	SPI_WriteData(0x05);
	SPI_WriteData(0x20);
	SPI_WriteData(0x21);
	SPI_WriteData(0x22);
	SPI_WriteData(0x23);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);


	GP_COMMAD_PA(39);
	SPI_WriteData(0xD6);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x19);
	SPI_WriteData(0x19);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x19);
	SPI_WriteData(0x19);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x05);
	SPI_WriteData(0x04);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x07);
	SPI_WriteData(0x06);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x03);
	SPI_WriteData(0x02);
	SPI_WriteData(0x23);
	SPI_WriteData(0x22);
	SPI_WriteData(0x21);
	SPI_WriteData(0x20);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);
	SPI_WriteData(0x18);




	GP_COMMAD_PA(2);
	SPI_WriteData(0xCC);
	SPI_WriteData(0x02);//02


	GP_COMMAD_PA(5);
	SPI_WriteData(0xC7);
	SPI_WriteData(0x00);
	SPI_WriteData(0x80);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);


	GP_COMMAD_PA(2);
	SPI_WriteData(0xD2);
	SPI_WriteData(0x99);


	GP_COMMAD_PA(2);
	SPI_WriteData(0x3A);
	SPI_WriteData(0x77);//77


	GP_COMMAD_PA(2);
	SPI_WriteData(0x11);
	SPI_WriteData(0x00);   
	Delay_us(5);



	GP_COMMAD_PA(2);
	SPI_WriteData(0x29);
	SPI_WriteData(0x00);   
	Delay_us(1);
                  
	//------------------------------------------------------------------------------------------	
	 
	//Cmd code 3: Access video mode 
//	//RGB interface configuration 
	SPI_2825_WrReg(0xb1,(LCD_HSPW<<8)|LCD_VSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(LCD_HBPD<<8)|LCD_VBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(LCD_HFPD<<8)|LCD_VFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	//SPI_2825_WrReg(0xb6, 0x0007);		//2007		//Video mode and video pixel format 	888
	SPI_2825_WrReg(0xb6, 0x0004);		//2007		//Video mode and video pixel format 	565
	//MIPI lane configuration
	//00 - 1 lane mode   
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode
	SPI_2825_WrReg(0xde, 0x0001);				//MIPI lane select, 2chl
	SPI_2825_WrReg(0xd6, 0x0005);				//Color order and endianess
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	
	Delay_ms(22);
	//PLL configuration
	//FR: bit15~14
	//00 每 62.5 < OUT f  < 125 
	//01 每 126 < OUT f  < 250 
	//10 每 251 < OUT f  < 500  
	//11 每 501 < OUT f  < 1000 
	
	//MS: bit12~8
	//Fpre = fin/MS
	
	//NS: bit7~0
	//Fout = Fpre*NS
	
	SPI_2825_WrReg(0xba, 0x8322);				//PLL setting,8028    
	SPI_2825_WrReg(0xbb, 0x0006);				//LP clock divider,煦けf/ㄗ1+1ㄘ,750MHZ/2 = 
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
	SPI_2825_WrReg(0xb7, 0x030B | 0x0020 );		//Generic mode, HS video mode	

}

#elif (Lcd_765_E00_480_854)
void Init_SSD2805_SPI(void)
{
	
 	SPI_Init();
	Delay_ms(10);
	Set_RST(0);// ( rGPFDAT &= (~(1<<3))) ;
	Delay_ms(50);
	Set_RST(1);//  ( rGPFDAT |= (1<<3) ) ;
	Delay_ms(10);

	
	//void SPI_2825_WrCmd(U8)
	//void SPI_2825_WrReg(U8 c,U16 value)
	//Initial code 1: SSD2825 initialization
	//RGB interface configuration 
	SPI_2825_WrReg(0xb1,(LCD_HSPW<<8)|LCD_VSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(LCD_HBPD<<8)|LCD_VBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(LCD_HFPD<<8)|LCD_VFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	//SPI_2825_WrReg(0xb6, 0x2007);				//Video mode and video pixel format 888
	SPI_2825_WrReg(0xb6, 0x2004);				//Video mode and video pixel format       565	
	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode 
	SPI_2825_WrReg(0xde, 0x0001);				//MIPI lane select 
	//SPI_2825_WrReg(0xDD, (LCD_HFPD<<8)|LCD_VFPD);
	//SPI_2825_WrReg(0xd6, 0x0004);				//Color order and endianess	BGR
	SPI_2825_WrReg(0xd6, 0x0005);				//Color order and endianess	RGB
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	//Communicate with LCD driver through MIPI 
	SPI_2825_WrReg(0xb7, 0x0342);				//DCS mode 0342
	SPI_2825_WrReg(0xb8, 0x0000);				//VC registe
	SPI_2825_WrReg(0xbc, 0x0000);				//Packet size 
	SPI_2825_WrCmd(0x11);					//LCD driver exit sleep mode
	Delay_ms(100);	
	SPI_2825_WrCmd(0x29);					//Set LCD driver display on 
//	PLL configuration 
	//SPI_2825_WrReg(0xba, 0x8334);				//PLL setting,8028 0x34--52*24/3=416M
	SPI_2825_WrReg(0xba, 0x8322);				//PLL setting,8028    0x27--39*24/3=312M
	
	SPI_2825_WrReg(0xbb, 0x0006);				//LP clock divider
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
//	SPI_2825_WrReg(0xb7, 0x030B);				//Generic mode, HS video mode
	
	Delay_ms(55);
	
	//Initial code 2: LCD driver initialization
	//MIPI lane and PLL configuration
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb7, 0x0150);				//Generic mode, LP mode
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register
	//Send command and data through MIPI 
	//------------------------------------------------------------------------------------------	
	//----------communicate with lcd driver through mipi----------------------------------------
	
	GP_COMMAD_PA(4);
	SPI_WriteData(0xBF);
	SPI_WriteData(0x91);
	SPI_WriteData(0x61);
	SPI_WriteData(0xF2); 

	GP_COMMAD_PA(3);
	SPI_WriteData(0xB3);
	SPI_WriteData(0x00);
	SPI_WriteData(0x77);

	GP_COMMAD_PA(3);
	SPI_WriteData(0xB4);
	SPI_WriteData(0x00);
	SPI_WriteData(0x77);

	GP_COMMAD_PA(7);
	SPI_WriteData(0xB8);
	SPI_WriteData(0x00);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x01);
	SPI_WriteData(0x00);
	SPI_WriteData(0xA0);
	SPI_WriteData(0x01);

	GP_COMMAD_PA(4);
	SPI_WriteData(0xBA);
	SPI_WriteData(0x3E);
	SPI_WriteData(0x23);
	SPI_WriteData(0x00);


	GP_COMMAD_PA(2);
	SPI_WriteData(0xC3);
	SPI_WriteData(0x04);


	GP_COMMAD_PA(3);
	SPI_WriteData(0xC4);
	SPI_WriteData(0x30);
	SPI_WriteData(0x6A);

	GP_COMMAD_PA(39);
	SPI_WriteData(0xC8);
	SPI_WriteData(0x7E);
	SPI_WriteData(0x68);
	SPI_WriteData(0x57);
	SPI_WriteData(0x49);
	SPI_WriteData(0x43);
	SPI_WriteData(0x33);
	SPI_WriteData(0x35);
	SPI_WriteData(0x1C);
	SPI_WriteData(0x33);
	SPI_WriteData(0x2F);
	SPI_WriteData(0x2B);
	SPI_WriteData(0x43);
	SPI_WriteData(0x2C);
	SPI_WriteData(0x31);
	SPI_WriteData(0x20);
	SPI_WriteData(0x22);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x1D);
	SPI_WriteData(0x03);
	SPI_WriteData(0x7E);
	SPI_WriteData(0x68);
	SPI_WriteData(0x57);
	SPI_WriteData(0x49);
	SPI_WriteData(0x43);
	SPI_WriteData(0x33);
	SPI_WriteData(0x35);
	SPI_WriteData(0x1C);
	SPI_WriteData(0x33);
	SPI_WriteData(0x2F);
	SPI_WriteData(0x2B);
	SPI_WriteData(0x43);
	SPI_WriteData(0x2C);
	SPI_WriteData(0x31);
	SPI_WriteData(0x20);
	SPI_WriteData(0x22);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x1D);
	SPI_WriteData(0x03);

	GP_COMMAD_PA(10);
	SPI_WriteData(0xC7);
	SPI_WriteData(0x00);
	SPI_WriteData(0x01);
	SPI_WriteData(0x31);
	SPI_WriteData(0x05);
	SPI_WriteData(0x65);
	SPI_WriteData(0x2B);
	SPI_WriteData(0x12);
	SPI_WriteData(0xA5);
	SPI_WriteData(0xA5);

	GP_COMMAD_PA(17);
	SPI_WriteData(0xD4);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x17);
	SPI_WriteData(0x37);
	SPI_WriteData(0x06);
	SPI_WriteData(0x04);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x08);
	SPI_WriteData(0x00);
	SPI_WriteData(0x02);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);

	GP_COMMAD_PA(17);
	SPI_WriteData(0xD5);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x17);
	SPI_WriteData(0x37);
	SPI_WriteData(0x07);
	SPI_WriteData(0x05);
	SPI_WriteData(0x0B);
	SPI_WriteData(0x09);
	SPI_WriteData(0x01);
	SPI_WriteData(0x03);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);

	GP_COMMAD_PA(17);
	SPI_WriteData(0xD6);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x13);
	SPI_WriteData(0x17);
	SPI_WriteData(0x17);
	SPI_WriteData(0x07);
	SPI_WriteData(0x09);
	SPI_WriteData(0x0B);
	SPI_WriteData(0x05);
	SPI_WriteData(0x03);
	SPI_WriteData(0x01);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);

	GP_COMMAD_PA(17);
	SPI_WriteData(0xD7);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1E);
	SPI_WriteData(0x17);
	SPI_WriteData(0x17);
	SPI_WriteData(0x06);
	SPI_WriteData(0x08);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x04);
	SPI_WriteData(0x02);
	SPI_WriteData(0x00);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x1F);

	GP_COMMAD_PA(21);
	SPI_WriteData(0xD8);
	SPI_WriteData(0x20);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x30);
	SPI_WriteData(0x03);
	SPI_WriteData(0x30);
	SPI_WriteData(0x01);
	SPI_WriteData(0x02);
	SPI_WriteData(0x00);
	SPI_WriteData(0x01);
	SPI_WriteData(0x02);
	SPI_WriteData(0x06);
	SPI_WriteData(0x70);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x73);
	SPI_WriteData(0x07);
	SPI_WriteData(0x06);
	SPI_WriteData(0x70);
	SPI_WriteData(0x08);

	GP_COMMAD_PA(21);
	SPI_WriteData(0xD9);
	SPI_WriteData(0x00);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x0A);
	SPI_WriteData(0x80);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x06);
	SPI_WriteData(0x7B);
	SPI_WriteData(0x00);
	SPI_WriteData(0x80);
	SPI_WriteData(0x00);
	SPI_WriteData(0x33);
	SPI_WriteData(0x6A);
	SPI_WriteData(0x1F);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x00);
	SPI_WriteData(0x03);
	SPI_WriteData(0x7B);

	GP_COMMAD_PA(2);
	SPI_WriteData(0x35);
	SPI_WriteData(0x00);

	GP_COMMAD_PA(2);
	SPI_WriteData(0x11);
	SPI_WriteData(0x00);   
	Delay_us(5);

	GP_COMMAD_PA(2);
	SPI_WriteData(0x29);
	SPI_WriteData(0x00);   
	Delay_us(1);
                  
	//------------------------------------------------------------------------------------------	
	 
	//Cmd code 3: Access video mode 
	//RGB interface configuration 
	SPI_2825_WrReg(0xb1,(LCD_HSPW<<8)|LCD_VSPW);	//Vertical sync and horizontal sync active period 
	SPI_2825_WrReg(0xb2,(LCD_HBPD<<8)|LCD_VBPD);	//Vertical and horizontal back porch period  
	SPI_2825_WrReg(0xb3,(LCD_HFPD<<8)|LCD_VFPD);	//Vertical and horizontal front porch period 
	SPI_2825_WrReg(0xb4, LCD_XSIZE_TFT);		//Horizontal active period 
	SPI_2825_WrReg(0xb5, LCD_YSIZE_TFT);		//Vertical active period
	//SPI_2825_WrReg(0xb6, 0x0007);		//2007		//Video mode and video pixel format 	888
	SPI_2825_WrReg(0xb6, 0x2004);		//2007		//Video mode and video pixel format 	565
	//MIPI lane configuration
	//00 - 1 lane mode 
	//01 - 2 lane mode 
	//10 - 3 lane mode 
	//11 - 4 lane mode
	SPI_2825_WrReg(0xde, 0x0001);				//MIPI lane select, 2chl
	SPI_2825_WrReg(0xd6, 0x0005);				//Color order and endianess
	SPI_2825_WrReg(0xb9, 0x0000);				//Disable PLL
	SPI_2825_WrReg(0xc4, 0x0001);				//BTA setting
	//CABC brightness setting 
	SPI_2825_WrReg(0xe9, 0xff2f);				//CABC control
	SPI_2825_WrReg(0xeb, 0x0100);				//CABC control
	
	Delay_ms(22);
	//PLL configuration
	//FR: bit15~14
	//00 每 62.5 < OUT f  < 125 
	//01 每 126 < OUT f  < 250 
	//10 每 251 < OUT f  < 500  
	//11 每 501 < OUT f  < 1000 
	
	//MS: bit12~8
	//Fpre = fin/MS
	
	//NS: bit7~0
	//Fout = Fpre*NS
	
	SPI_2825_WrReg(0xba, 0x8324);				//PLL setting,8028    
	SPI_2825_WrReg(0xbb, 0x0006);				//LP clock divider,煦けf/ㄗ1+1ㄘ,750MHZ/2 = 
	SPI_2825_WrReg(0xb9, 0x0001);				//PLL enable 
	SPI_2825_WrReg(0xb8, 0x0000);				//VC register 
	SPI_2825_WrReg(0xb7, 0x030B | 0x0020 );		//Generic mode, HS video mode	
}

#else
"error";
#endif



#endif





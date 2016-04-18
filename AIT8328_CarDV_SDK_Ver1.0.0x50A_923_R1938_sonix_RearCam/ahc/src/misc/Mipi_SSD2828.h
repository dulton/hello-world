/*******************************************************************************
 *
 *   UartShell.c
 *
 *   Interface for the UartShell class.
 *
 *   Copyright 2010 by Caesar Chang.
 *
 *   AUTHOR : Caesar Chang
 *
 *   VERSION: 1.0
 *
 *
*******************************************************************************/


#ifndef _MIPI_SSD2828_H_
#define _MIPI_SSD2828_H_

#if (ENABLE_MIPI_PANEL)

#define Lcd_95_A00_540_960		0
#define Lcd_765_E00_480_854		1

void Init_SSD2805_SPI(void);
#endif

#endif 

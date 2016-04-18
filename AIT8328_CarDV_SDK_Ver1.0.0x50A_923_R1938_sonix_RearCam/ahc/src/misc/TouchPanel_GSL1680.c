/*===========================================================================
 * Include files
 *===========================================================================*/

#include "mmph_hif.h"
#include "mmpf_pio.h"
#include "mmpf_i2cm.h"
#include "AIT_init.h"
#include "AHC_utility.h"
#include "mmpf_touchpanel.h"
#include "mmp_reg_pad.h"
#include "lcd_common.h"
//#include "gslX680_board_YueShen.h"
#include "gslX680_854.h"   //mipi lcd  and TP
#include "TouchPanel_GSL1680.h"
/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#if ( DSC_TOUCH_PANEL == TOUCH_PANEL_GSL1680 )
extern MMP_ULONG glAhcBootComplete;
static MMP_I2CM_ATTR  m_TouchPanelI2c = {MMP_I2CM2, TOUCH_PANEL_GSL_I2C_SLAVE_ADDRESS, 8, 8, 0x20, MMP_FALSE, MMP_FALSE, MMP_FALSE, MMP_FALSE, 0x0, 0x0, 0x0, 200000 /*250KHZ*/, MMP_TRUE, NULL, NULL, MMP_FALSE, MMP_FALSE, MMP_FALSE};

PGSL_TS g_pTs = NULL;

static MMP_ULONG id_sign[MAX_CONTACTS+1] = {0};
static MMP_UBYTE id_state_flag[MAX_CONTACTS+1] = {0};
static MMP_UBYTE id_state_old_flag[MAX_CONTACTS+1] = {0};
static MMP_USHORT x_old[MAX_CONTACTS+1] = {0};
static MMP_USHORT y_old[MAX_CONTACTS+1] = {0};
static MMP_USHORT x_new = 0;
static MMP_USHORT y_new = 0;
static MMP_USHORT x_PrevNew = 0;
static MMP_USHORT y_PrevNew = 0;
static MMP_USHORT x_Prev = 0;
static MMP_USHORT y_Prev = 0;

static MMP_BOOL m_bTouchRelease = FALSE;
static MMP_BOOL m_bTouchInitOk = FALSE;

static 
void TOUCH_ISR(MMP_GPIO_PIN piopin)
{
       MMP_ULONG  ulNow;
       static MMP_ULONG ulPre = 0;
	
	MMPF_OS_GetTime(&ulNow);
	if(m_bTouchRelease == TRUE)
	{
		if(ulNow - ulPre < 500)
		{
			return; 
		}
	}
	ulPre = ulNow;
	
	if(glAhcBootComplete == MMP_TRUE)
        	SetKeyPadEvent(TOUCH_PANEL_PRESS);
}

void InitTouchGpio( MMP_GPIO_PIN piopin,GpioCallBackFunc* CallBackFunc  )
{
    if(piopin == MMP_GPIO_MAX)
		return;
    
    printc("%s11 \r\n", __func__);
    //Set GPIO as input mode
    MMPF_PIO_EnableOutputMode(piopin, MMP_FALSE, MMP_TRUE);

    // Set the trigger mode.
    MMPF_PIO_EnableTrigMode(piopin, GPIO_L2H_EDGE_TRIG, MMP_TRUE, MMP_TRUE);

    //Enable Interrupt
    MMPF_PIO_EnableInterrupt(piopin, MMP_TRUE, 0, (GpioCallBackFunc *)CallBackFunc, MMP_TRUE);
}

MMP_ERR TouchPanel_Get_Reg_Data(MMP_USHORT RegStartAddr,MMP_USHORT* RegData,MMP_UBYTE ReadLen)
{
    if(!RegData) 
        return MMP_HIF_ERR_PARAMETER;
                  
    return MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c,RegStartAddr,RegData,ReadLen);
}

MMP_ERR TouchPanel_Set_Reg_Data(MMP_USHORT RegStartAddr,MMP_USHORT* RegData,MMP_UBYTE WriteLen)
{
    	if(!RegData || !WriteLen) 
    	{
    	  
        printc("%s,para error=%x  WriteLen:%d\r\n", __func__,RegData, WriteLen);
        return MMP_HIF_ERR_PARAMETER;
    	}
                  
    return MMPF_I2cm_WriteBurstData(&m_TouchPanelI2c,RegStartAddr,RegData,WriteLen);
}

MMP_ERR TouchPanel_Sleep(MMP_ULONG ulMs)
{
	MMPC_System_WaitMs(ulMs*4);
}

void TouchPanel_Reset()
{
    // Interrupt pin. Need to pull high to active.
    // Low   - Touch down
    // Hight - No touch

    MMPF_PIO_EnableOutputMode(TOUCH_RESET_GPIO, MMP_TRUE, MMP_TRUE);
	
    MMPF_PIO_SetData(TOUCH_RESET_GPIO, 0x00, MMP_TRUE);
    TouchPanel_Sleep(20);

    MMPF_PIO_SetData(TOUCH_RESET_GPIO, 0x01, MMP_TRUE);
    TouchPanel_Sleep(20);
}


MMP_ERR TouchPanel_I2c_Test()
{
    MMP_ERR ret = MMP_ERR_NONE;
    MMP_USHORT IcVersion[6] = {0};
    MMP_UBYTE u8Reg = 0xf0;
    MMP_USHORT u8ReadBuf = 0;
                  
    ret = MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, u8Reg, &u8ReadBuf, 1);
    if(ret == MMP_ERR_NONE)
    {
           printc("%s,u8ReadBuf = %x \r\n", __func__,u8ReadBuf);
    }
    else
     {
           printc("%s, MMPF_I2cm_ReadBurstData fail 11!\r\n", __func__);
	    goto lExit;
     }

     TouchPanel_Sleep(2);
     u8ReadBuf = 0x12;
     ret = TouchPanel_Set_Reg_Data(u8Reg, &u8ReadBuf, 1);
     if (ret != MMP_ERR_NONE)
     {
         printc("%s,write reg data failed \r\n", __func__);
     } 

     TouchPanel_Sleep(2);
     ret = MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, u8Reg, &u8ReadBuf, 1);
     if(ret == MMP_ERR_NONE)
     {
            printc("%s,u8ReadBuf = %x \r\n", __func__,u8ReadBuf);
     }
     else
     {
           printc("%s, MMPF_I2cm_ReadBurstData fail 22!\r\n", __func__);
   	    goto lExit;
     }

     printc("%s, GSL1680 I2c test ok!\r\n", __func__);
lExit:
    return ret;
}


void TouchPanel_clr_reg(void)
{
	MMP_USHORT write_buf[4]	= {0};

	write_buf[0] = 0x88;	
	TouchPanel_Set_Reg_Data(0xe0, &write_buf[0], 1);
       TouchPanel_Sleep(20);
	
	write_buf[0] = 0x03;
	TouchPanel_Set_Reg_Data(0x80, &write_buf[0], 1);
       TouchPanel_Sleep(5);
	
	write_buf[0] = 0x04;
	TouchPanel_Set_Reg_Data(0xe4, &write_buf[0], 1);
       TouchPanel_Sleep(5);
	
	write_buf[0] = 0x00;
	TouchPanel_Set_Reg_Data(0xe0, &write_buf[0], 1);
       TouchPanel_Sleep(20);
}


void TouchPanel_reset_chip(void)
{
	MMP_USHORT tmp = 0x88;
	MMP_USHORT buf[4] = {0x00};
	
	TouchPanel_Set_Reg_Data(0xe0, &tmp, 1);
       TouchPanel_Sleep(20);
	
	tmp = 0x04;
	TouchPanel_Set_Reg_Data(0xe4, &tmp, 1);
       TouchPanel_Sleep(10);
	
	TouchPanel_Set_Reg_Data(0xbc, buf, 4);
       TouchPanel_Sleep(10);
}


void TouchPanel_startup_chip(void)
{
	MMP_USHORT tmp = 0x00;
	MMP_USHORT buf[4] = {0x00};
#if 0	
	buf[3] = 0x01;
	buf[2] = 0xfe;
	buf[1] = 0x10;
	buf[0] = 0x00;	
	TouchPanel_Set_Reg_Data(0xf0, buf, 4);
	buf[3] = 0x00;
	buf[2] = 0x00;
	buf[1] = 0x00;
	buf[0] = 0x0f;	
	TouchPanel_Set_Reg_Data(0x04, buf, 4);
       TouchPanel_Sleep(20);
#endif
	TouchPanel_Set_Reg_Data(0xe0, &tmp, 1);
       TouchPanel_Sleep(10);
}


MMP_BOOL TouchPanel_gslX680_ts_init(void)
{
	MMP_BOOL bRet = TRUE;
	
	g_pTs = (PGSL_TS)malloc(sizeof(GSL_TS));
	if(g_pTs == NULL)
	{
		bRet = FALSE;
		goto lExit;
	}
	memset(g_pTs, 0, sizeof(GSL_TS));

	g_pTs->dd = (PGSL_TS_DATA)malloc(sizeof(GSL_TS_DATA));
	if(g_pTs->dd == NULL)
	{
		bRet = FALSE;
		goto lExit;
	}
	memset(g_pTs->dd, 0, sizeof(GSL_TS_DATA));

	g_pTs->dd->x_index = 6;
	g_pTs->dd->y_index = 4;
	g_pTs->dd->z_index = 5;
	g_pTs->dd->id_index = 7;
	g_pTs->dd->touch_index = 0;
	g_pTs->dd->data_reg = GSL_DATA_REG;
	g_pTs->dd->status_reg = GSL_STATUS_REG;
	g_pTs->dd->update_data = 0x4;
	g_pTs->dd->touch_bytes = 4;
	g_pTs->dd->touch_meta_data = 4;
	g_pTs->dd->finger_size = 70;

	g_pTs->dd->data_size = MAX_FINGERS * g_pTs->dd->touch_bytes + g_pTs->dd->touch_meta_data;
	g_pTs->touch_data = (MMP_USHORT*)malloc(g_pTs->dd->data_size);	
	if(g_pTs->touch_data == NULL)
	{
		bRet = FALSE;
		goto lExit;
	}
	memset(g_pTs->touch_data, 0, sizeof(g_pTs->dd->data_size));

	return bRet;

lExit:
	if(g_pTs != NULL)
	{
		if(g_pTs->dd != NULL)
		{
			free(g_pTs->dd);
			g_pTs->dd = NULL;
		}
		if(g_pTs->touch_data != NULL)
		{
			free(g_pTs->touch_data);
			g_pTs->touch_data = NULL;
		}
		free(g_pTs);
		g_pTs = NULL;
	}
	
	return bRet;
}

 void fw2buf(MMP_USHORT *buf,  const MMP_ULONG *fw)
{
#if 0
	MMP_ULONG *u32_buf;
	u32_buf = (MMP_ULONG *)buf;
	*u32_buf = *fw;
#else
	buf[0] = *fw & 0x000000ff;
	buf[1] = (*fw >> 8) & 0x000000ff;
	buf[2] = (*fw >> 16) & 0x000000ff;
	buf[3] = (*fw >> 24) & 0x000000ff;
#endif
}

#if 1
 void TouchPanel_gsl_load_fw(void)
{
	MMP_USHORT buf[DMA_TRANS_LEN*4 + 1] = {0};
	MMP_UBYTE send_flag = 1;
	MMP_USHORT *cur = buf;
	MMP_ULONG source_line = 0;
	MMP_ULONG source_len;
	const GSL_FW_DATA *ptr_fw = GSLX680_FW;
	MMP_LONG nCount = 0;
	MMP_LONG i = 0;
	MMP_USHORT len;
	MMP_BOOL  bCanPrint = TRUE;
	MMP_USHORT regValue;

	//ptr_fw = GSLX680_FW;
	source_len = sizeof(GSLX680_FW)/sizeof(GSLX680_FW[0]);//ARRAY_SIZE(GSLX680_FW);

	for (source_line = 0; source_line < source_len; source_line++) 
	//for (source_line = 0; source_line < 50; source_line++) 
	{
		/* init page trans, set the page val */
		if (GSL_PAGE_REG == ptr_fw[source_line].offset)
		{
			fw2buf(cur, &ptr_fw[source_line].val);
           		//printc("value00=0x%x, 0x%x, 0x%x, 0x%x, 0x%x\r\n", buf[0],  buf[1], buf[2], buf[3], buf[4]);	
			TouchPanel_Set_Reg_Data(GSL_PAGE_REG, buf, 4);
			send_flag = 1;	
		}
		else 
		{
			if (1 == send_flag % (DMA_TRANS_LEN < 0x20 ? DMA_TRANS_LEN : 0x20))
			{
	    			regValue = (MMP_UBYTE)ptr_fw[source_line].offset;
				//bCanPrint = TRUE;
			}

			fw2buf(cur, &ptr_fw[source_line].val);		
			cur += 4;

			if (0 == send_flag % (DMA_TRANS_LEN < 0x20 ? DMA_TRANS_LEN : 0x20)) 
			{
#if 0			
				if(bCanPrint == TRUE)
				{
					bCanPrint = FALSE;
					len = cur - buf;
	           			printc("reg=0x%x, len=%d!\r\n", regValue, len);
					for(i = 0; i < len; i++)
					{
						if((i % 4 == 0)   && (i > 0))
	             					printc("\r\nreg%d=0x", i/4);
	           				printc("%x", buf[i]);
					}
	             			printc("\r\n");
				}					
#endif						
				TouchPanel_Set_Reg_Data(regValue,  buf, cur - buf);
    				cur = buf;
			}

			send_flag++;
		}
		
	}

}
#endif
 

MMP_ERR TouchPanel_Gsl_Probe()
{
     MMP_ERR ret = MMP_ERR_NONE;

     TouchPanel_Reset();

     ret = TouchPanel_I2c_Test();
     if(MMP_ERR_NONE != ret)
     {
     		printc("[TouchPanel_I2c_Test  fail!!\r\n");
     		goto lExit;
     }
    
    TouchPanel_clr_reg();
    TouchPanel_reset_chip();
    TouchPanel_gsl_load_fw();	
    TouchPanel_startup_chip();	
    TouchPanel_reset_chip();	
    TouchPanel_startup_chip();

lExit:
	return ret;
}


MMP_ERR  TouchPanel_check_mem_data(void)
{
	MMP_USHORT read_buf[4]  = {0};
      MMP_UBYTE u8Reg = 0xb0;
                  
     TouchPanel_Sleep(30);
      MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, u8Reg, read_buf, 4);
	
	if (read_buf[3] != 0x5a || read_buf[2] != 0x5a || read_buf[1] != 0x5a || read_buf[0] != 0x5a)
	{
     		printc("check_mem_data error 0xb0 = %x %x %x %x !\r\n", read_buf[3], read_buf[2], read_buf[1], read_buf[0]);
		return TouchPanel_Gsl_Probe();
	}
	else
	{
     		printc("check_mem_data ok!! !\r\n");
	}

	return MMP_ERR_NONE;
}

MMP_ERR TouchPanel_Init(struct _3RD_PARTY_TOUCHPANEL *pthis)
{   
    MMP_USHORT ConfigVersion = 1;
        
    printc("[TouchPanel_Init] GSL1680\r\n");
  
     if(MMP_ERR_NONE != TouchPanel_Gsl_Probe())
     {
     		printc("[TouchPanel_Init11]GSL1680 init fail!!\r\n");
     		goto lExit;
     }
     
     if(MMP_ERR_NONE != TouchPanel_check_mem_data())
     {
     		printc("[TouchPanel_Init22]GSL1680 init fail!!\r\n");
     		goto lExit;
     }

     if(TouchPanel_gslX680_ts_init() == FALSE)
     {
     		printc("[TouchPanel_Init33]TouchPanel_gslX680_ts_init fail!!\r\n");
     		goto lExit;
    }

    m_bTouchInitOk = TRUE;
    InitTouchGpio(TOUCH_PANEL_INT_GPIO, (GpioCallBackFunc*) TOUCH_ISR );

lExit:
    return MMP_ERR_NONE;
}

void TouchPanel_TransformPosition(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir) // Set dir = 0 as lcd normal, 1: reverse.
{
	MMP_USHORT tpRefCenterX, tpRefCenterY,usPanelW,usPanelH;
	MMP_USHORT Temp;

#if (TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_0)
	if(dir)//LCD reverse
	{
		Temp = *pX;
		*pX = *pY;
		*pY = Temp;
	}
#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_1)
	if(dir)
	{
    	*pX = GSL_ADC_STEPS_X-*pX;
	}
    else
    {
        *pY = GSL_ADC_STEPS_Y-*pY;
    }
#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_2)
	if(dir)
	{
    	*pY = GSL_ADC_STEPS_Y-*pY;
	}
    else
    {
        *pX = GSL_ADC_STEPS_X-*pX;
    }
#elif(TOUCH_IC_PLACEMENT_TYPE == TOUCH_PANEL_DIR_3)
	if(!dir)//LCD Normal
	{
		*pX = GSL_ADC_STEPS_X-*pX;
		*pY = GSL_ADC_STEPS_Y-*pY;
	}
#endif

    //MAPPING TO RESOLUTION
    tpRefCenterX = *pX;
    tpRefCenterY = *pY;

#if (VERTICAL_LCD == 0)
    usPanelW = RTNA_LCD_GetAttr()->usPanelW;
    usPanelH = RTNA_LCD_GetAttr()->usPanelH;
#else
    usPanelW = RTNA_LCD_GetAttr()->usPanelH;
    usPanelH = RTNA_LCD_GetAttr()->usPanelW;
#endif

    *pX = (tpRefCenterX * usPanelW)/GSL_ADC_STEPS_X;
    *pY = (tpRefCenterY * usPanelH)/GSL_ADC_STEPS_Y;
    printc("%s,pX=%d,pY=%d dir=%d\r\n", __func__,*pX,*pY, dir);
}


static MMP_USHORT join_bytes(MMP_UBYTE a, MMP_UBYTE b)
{
	MMP_USHORT ab = 0;
	ab = ab | a;
	ab = ab << 8 | b;
	return ab;
}

static void filter_point(MMP_USHORT x, MMP_USHORT y , MMP_UBYTE id)
{
	MMP_USHORT x_err =0;
	MMP_USHORT y_err =0;
	MMP_USHORT filter_step_x = 0, filter_step_y = 0;
	
	id_sign[id] = id_sign[id] + 1;
	if(id_sign[id] == 1)
	{
		x_old[id] = x;
		y_old[id] = y;
	}
	
	x_err = x > x_old[id] ? (x -x_old[id]) : (x_old[id] - x);
	y_err = y > y_old[id] ? (y -y_old[id]) : (y_old[id] - y);

	if( (x_err > FILTER_MAX && y_err > FILTER_MAX/3) || (x_err > FILTER_MAX/3 && y_err > FILTER_MAX) )
	{
		filter_step_x = x_err;
		filter_step_y = y_err;
	}
	else
	{
		if(x_err > FILTER_MAX)
			filter_step_x = x_err; 
		if(y_err> FILTER_MAX)
			filter_step_y = y_err;
	}

	if(x_err <= 2*FILTER_MAX && y_err <= 2*FILTER_MAX)
	{
		filter_step_x >>= 2; 
		filter_step_y >>= 2;
	}
	else if(x_err <= 3*FILTER_MAX && y_err <= 3*FILTER_MAX)
	{
		filter_step_x >>= 1; 
		filter_step_y >>= 1;
	}	
	else if(x_err <= 4*FILTER_MAX && y_err <= 4*FILTER_MAX)
	{
		filter_step_x = filter_step_x*3/4; 
		filter_step_y = filter_step_y*3/4;
	}	
		
	x_new = x > x_old[id] ? (x_old[id] + filter_step_x) : (x_old[id] - filter_step_x);
	y_new = y > y_old[id] ? (y_old[id] + filter_step_y) : (y_old[id] - filter_step_y);

	x_old[id] = x_new;
	y_old[id] = y_new;
}

void gslSwap(MMP_USHORT *x, MMP_USHORT *y)
{
	MMP_USHORT temp;

	temp = *y;
	*y = *x;
	*x = temp;
}
	
int TouchPanel_ReadPosition(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir)
{
	MMP_ERR ret = MMP_ERR_NONE; 
       MMP_ULONG  ulNow;
	static MMP_ULONG ulPre = 0;
#if 1
	MMP_UBYTE id, touches = 0;
	MMP_USHORT x, y, i;
	PGSL_TS ts;
	ts = g_pTs;

	if(m_bTouchInitOk == FALSE)
	{
      		printc("%s, GSL1680 have not initlize! \r\n", __func__);
		return ret;
	}

	ret = MMPF_I2cm_ReadBurstData(&m_TouchPanelI2c, 0x80, ts->touch_data, ts->dd->data_size);
	if(ret != MMP_ERR_NONE)
	{
           printc("%s, read i2c data fail!\r\n", __func__);
	    goto lExit;
	}
		
	touches = ts->touch_data[ts->dd->touch_index];
#if 1
	//if(touches != 0 && touches != 1 && touches != 2)
	if(touches != 0 && touches != 1)
	{
             printc("gslX680_ts_worker touches: %d!\r\n", touches);
		touches = 0; //add for 偶尔发现没有pen up消息 这个时候都出现[_I2C_WaitAck] error,status=48! touch =255
	       goto lExit;
	}
#endif	
	
	for(i = 1; i <= MAX_CONTACTS; i ++)
	{
		if(touches == 0)
			id_sign[i] = 0;	
		
		id_state_flag[i] = 0;
	}
	
#if 0
	x = join_bytes( ( ts->touch_data[ts->dd->x_index  + 4 * 0 + 1] & 0xf),
					ts->touch_data[ts->dd->x_index + 4 * 0]);

	y = join_bytes(ts->touch_data[ts->dd->y_index + 4 * 0 + 1],
			ts->touch_data[ts->dd->y_index + 4 * 0 ]);
	if(touches == 2)
	{
		x += join_bytes( ( ts->touch_data[ts->dd->x_index  + 4 * 1 + 1] & 0xf),
				ts->touch_data[ts->dd->x_index + 4 * 1]);

		y += join_bytes(ts->touch_data[ts->dd->y_index + 4 * 1 + 1],
				ts->touch_data[ts->dd->y_index + 4 * 1 ]);
		x /= 2;
		y /= 2;
	}

	id = ts->touch_data[ts->dd->id_index + 4 * 0] >> 4;

         printc("x:%d y:%d id:%d!\r\n", x, y, id);
	//if(1 <=id && id <= MAX_CONTACTS)
	{
		filter_point(x, y ,id);	
		id_state_flag[id] = 1;
	}
	
	gslSwap(&x_new,  &y_new);
	y_new = GSL_ADC_STEPS_X - y_new;   //2015.12.22  new board
#else
	for(i= 0;i < touches; i ++)
	{
		x = join_bytes( ( ts->touch_data[ts->dd->x_index  + 4 * i + 1] & 0xf),
					ts->touch_data[ts->dd->x_index + 4 * i]);

		y = join_bytes(ts->touch_data[ts->dd->y_index + 4 * i + 1],
				ts->touch_data[ts->dd->y_index + 4 * i ]);	
		
		id = ts->touch_data[ts->dd->id_index + 4 * i] >> 4;

             //printc("x:%d y:%d id:%d!\r\n", x, y, id);
		//if(1 <=id && id <= MAX_CONTACTS)
		{
			filter_point(x, y ,id);	
			id_state_flag[id] = 1;
		}
		
		gslSwap(&x_new,  &y_new);
		y_new = GSL_ADC_STEPS_X - y_new;   //2015.12.22  new board
	}
#endif

	for(i = 1; i <= MAX_CONTACTS; i ++)
	{	
		if( (0 == touches) || ((0 != id_state_old_flag[i]) && (0 == id_state_flag[i])) )
		{
			id_sign[i]=0;
		}
		id_state_old_flag[i] = id_state_flag[i];
	}

lExit:	
	
	/*if(x_new > DISP_WIDTH || y_new > DISP_HEIGHT)
	{
		touches = 0;
	}
	else*/
	{
		*pX = x_new/OSD_DISPLAY_SCALE_LCD;
		*pY = y_new/OSD_DISPLAY_SCALE_LCD;
	}
	
	if(touches == 1)
	{
		m_bTouchRelease = FALSE;
	}
	else
	{
		m_bTouchRelease = TRUE;
	}
    	//TouchPanel_TransformPosition(pX,pY,dir);
       printc("gslX680_Read x:%d y:%d touches:%d!\r\n", *pX, *pY, touches);

	return touches;
#endif
}


MMP_ERR TouchPanel_ReadVBAT1(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir )
{
	return MMP_ERR_NONE;
}


void TouchPanel_ResetCtrl(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_BOOL bOn)
{
    printc("TouchPanel_ResetCtrl : bon %d\r\n",bOn);

#ifdef TOUCH_RESET_GPIO
	if (TOUCH_RESET_GPIO != MMP_GPIO_MAX)
	{
        //TouchPanel_Init(pthis);
		//MMPF_PIO_EnableOutputMode(TOUCH_RESET_GPIO, MMP_TRUE, MMP_TRUE);
		//MMPF_PIO_SetData(TOUCH_RESET_GPIO, (bOn ? TOUCH_RESET_GPIO_ACT_MODE : !TOUCH_RESET_GPIO_ACT_MODE), MMP_TRUE);
	}
#endif

}

MMP_BOOL TouchPanel_Attached(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
    return AHC_TRUE;
}

MMP_ERR TouchPanel_IOControl(struct _3RD_PARTY_TOUCHPANEL *pthis, TOUCHPANEL_ATTR *ptouchpanel_attr, MMP_UBYTE cmd, MMP_UBYTE* arg)
{
    return MMP_ERR_NONE;
}

struct _3RD_PARTY_TOUCHPANEL TouchPanel_Obj =
{
    TouchPanel_Init,
    TouchPanel_ReadPosition,
    TouchPanel_ReadVBAT1,
    TouchPanel_ResetCtrl,
    TouchPanel_IOControl,
    TouchPanel_Attached,
};

int Touch_Panel_Module_Init(void)
{
    printc("[Touch_Panel_Module_Init_1680]\r\n");
    MMPF_3RDParty_TouchPanel_Register(&TouchPanel_Obj);
    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6"
#pragma O0
ait_module_init(Touch_Panel_Module_Init);
#pragma
#pragma arm section rodata, rwdata ,  zidata

#endif //TOUCH_PANEL_FT6X36

/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "Customer_Config.h"
#include "config_fw.h"
#include "mmpf_i2cm.h"
#include "os_wrap.h"
#include "mmpf_pio.h"
#include "mmpf_timer.h"
#include "lib_retina.h"
#include "AHC_utility.h"
#include "AHC_Fs.h"

#if (GSENSOR_CONNECT_ENABLE && GSENSOR_MODULE == GSENSOR_MODULE_GMA305)

#include "Gsensor_gma305.h"
#include "AIT_Init.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define	TO_NEGATIVE_VALUE(x)	( (~x + 0x01) | 0x80 )
#define	TO_ABSOLUTE_VALUE(x)	( (x > 0x4000)? (0x4000 - (x&0x3FFF)):(x) )	

#define	LOW_BOUND_1G(s)			((signed char)s->ub1G - (signed char)s->ub1G_Err)
#define ABS(x) 					((x) > 0 ? (x) : -(x))

#define	MAP2HW_G(g, s)			((g * s->ub1G) / SCALE_G)

#define	STABLE_CNT				(0)

#define moving_step 8
#define SENSOR_DATA_SIZE 		3 


/*===========================================================================
 * Global varible
 *===========================================================================*/
INT32	bufferave[3][moving_step];	// 16 buffer
INT32	sum[3];		//  sum of moving_step 
INT32	avg[3];		// moving average result
INT32 OldGsXData = 0;
INT32 OldGsYData = 0;
INT32 OldGsZData = 0;
UINT8 pointer = 0;
static GSENSOR_DATA			m_GSensorOffset 		= {0, 0, 0};
//GSNR_ATTRIBUTE       		gGsensor_Attribute 		= {2, 6, 2, NULL};
//UINT8						bGsensorID				= 0xFF;

static int					kUpCnt 					= 0;
static int					kDwCnt 					= 0;
static int					kLastPosition 			= GSNR_POSITION_UNKNOWN;

MMP_BOOL	 				bGsensorinitialized 	= MMP_FALSE;

// I2cm Attribute
static MMP_I2CM_ATTR m_I2cmAttr_gsensor = {
	MMP_I2CM1,
    GSENSOR_SLAVE_ADDR << 1,
    8,
    8,
    0x02,
    MMP_FALSE,
    MMP_FALSE,
    MMP_FALSE,
    MMP_FALSE,
    0,
    0,
    1,
    200000,
    MMP_TRUE,
    GSENSOR_SCL,
    GSENSOR_SDA,
    MMP_FALSE,
    MMP_FALSE,
	MMP_FALSE,
	0                   //ubVifPioMdlId
};

/*===========================================================================
 * Extern function
 *===========================================================================*/

#if (GSENSOR_DBG_MSG && defined(ALL_FW))
extern void	printc( char* szFormat, ... );
#define	DPRINT( ... ) printc( __VA_ARGS__ )
#else
#define	DPRINT( ... ) {}
#endif

MMP_ERR Gsensor_EnableIntXYZ(int ex, int ey, int ez);

/*===========================================================================
 * Main body
 *===========================================================================*/
#ifdef ALL_FW
MMP_ERR Gsensor_Config_I2CM(struct _3RD_PARTY_GSENSOR *pthis, GSNR_I2CM_ATTRIBUTE *psI2cmattr)
{
	m_I2cmAttr_gsensor.i2cmID 		= psI2cmattr->i2cmID;
	m_I2cmAttr_gsensor.ubPadNum 	= psI2cmattr->ubPadNum;
	m_I2cmAttr_gsensor.ulI2cmSpeed  = psI2cmattr->ulI2cmSpeed;

    return MMP_ERR_NONE;
}

MMP_ERR Gsensor_ReadRegister(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE reg , MMP_UBYTE* pval)
{
	MMP_USHORT data;

	if(reg >= GSensor_Reg_INVALID)
	{
		DPRINT("GsnrRead-->Wrong Register Address\r\n");
		return MMP_ERR_NONE;
	}
    
	MMPF_I2cm_ReadReg(&m_I2cmAttr_gsensor, (MMP_USHORT)reg, (MMP_USHORT*)&data);
	
	*pval = (MMP_UBYTE)(data & 0x00FF);

	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_ReadRegister_2byte(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE reg , MMP_USHORT* pval)
{
	MMP_USHORT data;

	if(reg >= GSensor_Reg_INVALID)
	{
		RTNA_DBG_Str(0, "GsnrRead-->Wrong Register Address\r\n");
		return MMP_ERR_NONE;
	}

	MMPF_I2cm_ReadReg(&m_I2cmAttr_gsensor, (MMP_USHORT)reg, (MMP_USHORT*)&data);
		
	*pval = data;

	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_WriteRegister(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE reg , MMP_UBYTE val)
{
	if (reg >= GSensor_Reg_INVALID)
	{
		RTNA_DBG_Str(0, FG_RED("GMA301: GsnrWrite-->Wrong Register Address\r\n"));
		return MMP_ERR_NONE;
	}
	
	MMPF_I2cm_WriteReg(&m_I2cmAttr_gsensor, (MMP_USHORT)reg, (MMP_USHORT)val);
	
	return MMP_ERR_NONE;
}

void Gsensor_DumpRegister(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr)
{
}

MMP_ERR Gsensor_ReadOffsetFromFile(struct _3RD_PARTY_GSENSOR *pthis)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_WriteOffsetToFile(struct _3RD_PARTY_GSENSOR *pthis)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_ResetDevice(struct _3RD_PARTY_GSENSOR *pthis) 
{
	MMP_ERR 	status 	= MMP_ERR_NONE;
	MMP_UBYTE 	value 	= 0x02;
    DPRINT(FG_GREEN("GMA305 Gsensor_ResetDevice -\r\n"));
	
	status |= pthis->MMPF_Gsensor_SetReg(pthis, 0x01, value);
	bGsensorinitialized = (status == MMP_ERR_NONE)?(MMP_TRUE):(MMP_FALSE);
	
	return status; 
}

MMP_ERR Gsensor_ReadDeviceID(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr)
{
	MMP_ERR 	status 		= MMP_ERR_NONE;
	MMP_USHORT	usdeviceID 	= 0;
	
#if 1	// by yoko 20150209
	status = Gsensor_ReadRegister_2byte(pthis, 0x04, &usdeviceID);
	
	usdeviceID &= 0x00FF;
	
	if(usdeviceID != GSENSOR_ID_GMA305)
#else
	status |= pthis->MMPF_Gsensor_GetReg(pthis, 0x00, &usdeviceID);
	
	usdeviceID &= 0x00FF;
	
	if(usdeviceID != GSENSOR_ID_GMA305)
#endif
	{
		status = 1;	// ERROR
		DPRINT("Gsensor Read Device ID 0x%x  %d Error\r\n",usdeviceID, status);
		pgsnr_attr->ubGsensorID = 0xFF;
	}
	else
	{
		status = 0;	// SUCCESS
		DPRINT("Gsensor Read Device ID 0x%x %d Success\r\n",usdeviceID, status);
		pgsnr_attr->ubGsensorID = (MMP_UBYTE)usdeviceID;
	}
	
	return status; 
}

MMP_ERR	Gsensor_GetSensorOffset(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE *data)
{
	return MMP_ERR_NONE;
}

MMP_ERR	Gsensor_SetSensorOffset(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE *data)
{
	return MMP_ERR_NONE;
}

static MMP_SHORT KeepGsensorValue[3] = {0};
MMP_ERR Gsensor_Read_XYZ(struct _3RD_PARTY_GSENSOR *pthis, GSENSOR_DATA *data) 
{
	MMP_UBYTE	i = 0;
	MMP_USHORT Regdata = 0;
    MMP_USHORT g_Data_L=0,g_Data_H=0;
	
	Gsensor_ReadRegister_2byte(pthis, 0x04, &Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x05, (MMP_UBYTE*)&Regdata);

	pthis->MMPF_Gsensor_GetReg(pthis, 0x06, (MMP_UBYTE*)&g_Data_L);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x07, (MMP_UBYTE*)&g_Data_H);
	g_Data_L &= 0x00FE;
	g_Data_H &= 0x00FF;
	//printc("Reg0x07 = 0x%X\r\n", (MMP_UBYTE)g_Data_H);
	//printc("Reg0x06 = 0x%X\r\n", (MMP_UBYTE)g_Data_L);
	#if 0
	data->acc_val[X_AXIS] = (g_Data_H<<8) | (g_Data_L);
	data->acc_val[X_AXIS] = data->acc_val[X_AXIS]>>1;
	data->acc_val[X_AXIS] = TO_ABSOLUTE_VALUE(data->acc_val[X_AXIS]);
	#endif
	#if 1
	data->acc_val[X_AXIS] = ((MMP_UBYTE)g_Data_H<<3)| ((MMP_UBYTE)g_Data_L>>(8-3)); // 11,16,14
	#endif
	
	pthis->MMPF_Gsensor_GetReg(pthis, 0x08, (MMP_UBYTE*)&g_Data_L);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x09, (MMP_UBYTE*)&g_Data_H);
	g_Data_L &= 0x00FE;
	g_Data_H &= 0x00FF;
	//printc("Reg0x09 = 0x%X\r\n", (MMP_UBYTE)g_Data_H);
	//printc("Reg0x08 = 0x%X\r\n", (MMP_UBYTE)g_Data_L);
	#if 0
	data->acc_val[Y_AXIS] = (g_Data_H<<8) | (g_Data_L);
	data->acc_val[Y_AXIS] = data->acc_val[Y_AXIS]>>1;
	data->acc_val[Y_AXIS] = TO_ABSOLUTE_VALUE(data->acc_val[Y_AXIS]);
    #endif	
	#if 1
	data->acc_val[Y_AXIS] = ((MMP_UBYTE)g_Data_H<<3)| ((MMP_UBYTE)g_Data_L>>(8-3)); 
	#endif
	
	pthis->MMPF_Gsensor_GetReg(pthis, 0x0A, (MMP_UBYTE*)&g_Data_L);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x0B, (MMP_UBYTE*)&g_Data_H);
	g_Data_L &= 0x00FE;
	g_Data_H &= 0x00FF;
	//printc("Reg0x0B = 0x%X\r\n", (MMP_UBYTE)g_Data_H);
	//printc("Reg0x0A = 0x%X\r\n", (MMP_UBYTE)g_Data_L);
    #if 0
	data->acc_val[Z_AXIS] = (g_Data_H<<8) | (g_Data_L);
	data->acc_val[Z_AXIS] = data->acc_val[Z_AXIS]>>1;
	data->acc_val[Z_AXIS] = TO_ABSOLUTE_VALUE(data->acc_val[Z_AXIS]);
    #endif
	#if 1
	data->acc_val[Z_AXIS] = ((MMP_UBYTE)g_Data_H<<3)| ((MMP_UBYTE)g_Data_L>>(8-3)); 
	#endif
	
#if 1
	for(i = 0; i < 3; i++)
	{ 
		if(KeepGsensorValue[i])
		{
			MMP_USHORT tempValue = 0;

			tempValue = data->acc_val[i];

			if(data->acc_val[i] > KeepGsensorValue[i])
				data->acc_val[i] = data->acc_val[i] - KeepGsensorValue[i];
			else
				data->acc_val[i] = KeepGsensorValue[i] - data->acc_val[i];

			//
			KeepGsensorValue[i] = tempValue;
		}
		else
		{
			KeepGsensorValue[i] = data->acc_val[i];
		}
	} 
#endif

	//printc("x %d, y %d, c %d\n",(MMP_USHORT)data->acc_val[X_AXIS], (MMP_USHORT)data->acc_val[Y_AXIS], (MMP_USHORT)data->acc_val[Z_AXIS]);
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_Read_XYZ_WithCompensation(struct _3RD_PARTY_GSENSOR *pthis, GSENSOR_DATA *sd) 
{
	pthis->MMPF_Gsensor_Read_XYZ(pthis, sd);
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_Read_Accel_Avg(struct _3RD_PARTY_GSENSOR *pthis, GSENSOR_DATA *data)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_CalculateOffset(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE gAxis, GSENSOR_DATA avg)
{
	return MMP_ERR_NONE;
}

MMP_ERR	Gsensor_Calibration(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE side)
{
	GSENSOR_DATA avg;
	
	pthis->MMPF_Gsensor_Read_Accel_Avg(pthis, &avg);
	
	pthis->MMPF_Gsensor_CalculateOffset(pthis, side, avg); 
	
	return MMP_ERR_NONE;
}

MMP_ERR	Gsensor_SetIntDuration(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE arg)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_GetIntStatus(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE* pstatus)
{
#if 0
	MMP_USHORT Regdata = 0;

	Gsensor_WriteRegister(0x16,0x1B);//Configured motion detection to differential mode and latch reference.
	Gsensor_WriteRegister(0x03,0x01);//Micro motion threshold from 0x01 ~ 0x1F only. 1 LSB = 0.25G. 4:1G   1F:7.75G

	Gsensor_ReadRegister_2byte(0x04, &Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x05, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x06, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x07, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x08, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x09, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x0A, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x0B, (MMP_UBYTE*)&Regdata);

	Gsensor_WriteRegister(0x15,0x0C);//Enable motion interrupt and set INT pin to open-drain and active low.0x0C high

	printc("GMA301C Gsensor int init finish\r\n");
#endif
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_SetPowerSaveMode(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE mode)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_SetNormalModeDataRate(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE rate)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_SetInterruptSource(struct _3RD_PARTY_GSENSOR *pthis, MMP_UBYTE src)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_GetDataRate(struct _3RD_PARTY_GSENSOR *pthis, MMP_USHORT *wDataRate)
{
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_SetDynamicRange(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE ubDrange)
{
	MMP_UBYTE ubFullSacleRange;

	ubFullSacleRange &= 0xFF;
	ubFullSacleRange |= DYNAMIC_RANGE_8;
	
 	pgsnr_attr->nMaxG	= MAX_16G;
 	pgsnr_attr->ub1G	 	= 64;	//0x10 * 4 = 64LSB (1G)
 	pgsnr_attr->ub1G_Err = 0x02;

	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_GetDynamicRange(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE *ubDrange)
{
	*ubDrange = 6;
	return MMP_ERR_NONE;
}

MMP_ERR Gsensor_EnableIntXYZ(int ex, int ey, int ez)
{
	MMP_ERR 	status 	= MMP_ERR_NONE;
	return status;
}

MMP_ERR Gsensor_SetGroundAxis(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE arg)
{
	MMP_ERR status = MMP_ERR_NONE;

	if ((arg != X_AXIS) &&
	    (arg != Y_AXIS) &&
	    (arg != Z_AXIS))
		status = 1;  // ERROR!
	else
		pgsnr_attr->ubGroundAxis = (MMP_UBYTE)arg;

	return status;
}

MMP_ERR Gsensor_Initial(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr)
{		
#if 0
	pthis->MMPF_Gsensor_SetReg(pthis, 0x01,0x02);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x03,0x1F);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x38,0x9F);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x15,0x08);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x16,0x89);
#endif	

#if 1
{
    MMP_UBYTE Regdata = 0;
	
    pthis->MMPF_Gsensor_GetReg(pthis, 0x00, (MMP_UBYTE*)&Regdata);
	printc("Gsensor_Initial: Regdata = 0x%X\r\n", Regdata);
	
	pthis->MMPF_Gsensor_SetReg(pthis, 0x18, 0x40);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x15, 0x2A);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x16, 0x00);
	pthis->MMPF_Gsensor_SetReg(pthis, 0x38, 0x5F);
}	
#endif
}

void Gsensor_Initial_PowerOff(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE ubPowerGsenOnValue)
{

}

void Gsensor_Int_Initial(void)
{
#if 0
	MMP_USHORT Regdata = 0;

	pthis->MMPF_Gsensor_SetReg(pthis, 0x16,0x1B);//Configured motion detection to differential mode and latch reference.
	pthis->MMPF_Gsensor_SetReg(pthis, 0x03,0x01);//Micro motion threshold from 0x01 ~ 0x1F only. 1 LSB = 0.25G. 4:1G   1F:7.75G

	Gsensor_ReadRegister_2byte(0x04, &Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x05, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x06, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x07, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x08, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x09, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x0A, (MMP_UBYTE*)&Regdata);
	pthis->MMPF_Gsensor_GetReg(pthis, 0x0B, (MMP_UBYTE*)&Regdata);

	pthis->MMPF_Gsensor_SetReg(pthis, 0x15,0x4c);// 0x4c / 5d//Enable motion interrupt and set INT pin to open-drain and active low.0x0C high
#endif	
}

MMP_ERR Gsensor_IOControl(struct _3RD_PARTY_GSENSOR *pthis, GSNR_ATTRIBUTE *pgsnr_attr, MMP_UBYTE cmd, MMP_UBYTE* arg)
{
	MMP_ERR status = MMP_ERR_NONE;
	
	switch(cmd)
	{
		case GSNR_CMD_RESET: 
			status |= pthis->MMPF_Gsensor_Reset(pthis);
			status |= pthis->MMPF_Gsensor_ReadDeviceID(pthis, pgsnr_attr); 
			if (status == MMP_ERR_NONE)
			{
				// Init default setting
				pgsnr_attr->ubGroundAxis = AXIS_UNKNOWN;
				pthis->MMPF_Gsensor_Initialize(pthis, pgsnr_attr);
				pgsnr_attr->nInited = 1;
			}
			else
			{
				pgsnr_attr->nInited = 0;
			}
		break;

		case GSNR_CMD_CALIBRATION: 
			status |= pthis->MMPF_Gsensor_Calibration(pthis, *arg);
		break;
		
		case GSNR_CMD_GET_OFFSET: 
			status |= pthis->MMPF_Gsensor_ReadOffsetFromFile(pthis);
			//status |= pthis->MMPF_Gsensor_GetSensorOffset(arg);
		break;
		
		case GSNR_CMD_SET_OFFSET: 
			status |= pthis->MMPF_Gsensor_SetSensorOffset(pthis, arg);
			status |= pthis->MMPF_Gsensor_WriteOffsetToFile(pthis);
		break;
		
		case GSNR_CMD_READ_ACCEL_XYZ:
			if(pgsnr_attr->nInited == 1)
    			status |= pthis->MMPF_Gsensor_Read_XYZ(pthis, (GSENSOR_DATA*)arg);
		break;

		case GSNR_CMD_READ_ACCEL_WITH_COMPENSATION:
			if(pgsnr_attr->nInited == 1)
    			status |= pthis->MMPF_Gsensor_Read_XYZ_WithCompensation(pthis, (GSENSOR_DATA*)arg);
		break;

		case GSNR_CMD_SET_PWR_ON_INT_THD: 
			Gsensor_Initial_PowerOff(pthis, pgsnr_attr, *arg);
		break;

		case GSNR_CMD_SET_INT_THD: 
		#if 0
		{
			status |= Gsensor_ResetDevice();
			status |= Gsensor_ReadDeviceID();
			if (status == MMP_ERR_NONE)			
			{
				printc("GSNR_CMD_SET_INT_THD \n");
				Gsensor_Int_Initial();
			}
			break;
		}
		#endif
		break;

		case GSNR_CMD_SET_INT_DURATION: 
			status |= pthis->MMPF_Gsensor_SetIntDuration(pthis, *arg);
		break;
		
		case GSNR_CMD_GET_INT_STATUS:
			if (pgsnr_attr->nInited) {
			    status |= pthis->MMPF_Gsensor_GetIntStatus(pthis, arg);
			} else {
				status = 1;	// ERROR!
				*arg   = 0;
			}
		break;
		
		case GSNR_CMD_SET_POWER_SAVE:
			status |= pthis->MMPF_Gsensor_SetPowerSaveMode(pthis, *arg);
		break;
		case GSNR_CMD_SET_NORMAL_MODE:
			//status |= pthis->MMPF_Gsensor_SetNormalModeDataRate(pthis, *arg);
		break;	
		case GSNR_CMD_SET_DATA_RATE:
			status |= pthis->MMPF_Gsensor_SetNormalModeDataRate(pthis, *arg);
		break;

		case GSNR_CMD_SET_INT_SOURCE:
			status |= pthis->MMPF_Gsensor_SetInterruptSource(pthis, *arg);
		break;
		
		case GSNR_CMD_WRITE_REG:
			status |= pthis->MMPF_Gsensor_SetReg(pthis, *arg , *(arg+1));
		break;
		
		case GSNR_CMD_READ_REG:
			status |= pthis->MMPF_Gsensor_GetReg(pthis, *arg , arg+1);
		break;
		
		case GSNR_CMD_GET_DATA_RATE:
			status |= pthis->MMPF_Gsensor_GetDataRate(pthis, (MMP_USHORT*)arg);
		break;
		
		case GSNR_CMD_SET_DYNA_RANGE:
			status |= pthis->MMPF_Gsensor_SetDynamicRange(pthis, pgsnr_attr, *arg);
		break;
		
		case GSNR_CMD_GET_DYNA_RANGE:
			status |= pthis->MMPF_Gsensor_GetDynamicRange(pthis, pgsnr_attr, arg);
		break;
		
		case GSNR_CMD_SET_GROUND_AXIS:
			status |= pthis->MMPF_Gsensor_SetGroundAxis(pthis, pgsnr_attr, (int)arg);
		break;		
		
		case GSNR_CMD_GROUND_AXIS_STATUS: 
		{
			GSENSOR_DATA	sd;
			GSNR_SBYTE		g;

			if (pgsnr_attr->ubGroundAxis == AXIS_UNKNOWN) {
				*arg = GSNR_POSITION_NOCHANGE;
				return MMP_ERR_NONE;
            }

			pthis->MMPF_Gsensor_Read_XYZ(pthis, &sd);
			
			g = (signed char)sd.acc_val[pgsnr_attr->ubGroundAxis];
			
			if (ABS(g) > LOW_BOUND_1G(pgsnr_attr)) 
			{
				if (g > 0) 
				{
					if ((kLastPosition != GSNR_POSITION_UP ||
						 kLastPosition == GSNR_POSITION_UNKNOWN) &&
						kUpCnt >= STABLE_CNT) 
					{
						kLastPosition = GSNR_POSITION_UP;
						*arg = kLastPosition;
						DPRINT("GSNR_POSITION_UP\r\n");
						return MMP_ERR_NONE;
					}

					if (kUpCnt < STABLE_CNT) 
					{
						kDwCnt = 0;
						kUpCnt++;
					}
				} 
				else 
				{
					if ((kLastPosition != GSNR_POSITION_DOWN ||
						 kLastPosition == GSNR_POSITION_UNKNOWN) &&
						 kDwCnt >= STABLE_CNT) 
					{
						kLastPosition = GSNR_POSITION_DOWN;
						*arg = kLastPosition;
						DPRINT("GSNR_POSITION_DOWN\r\n");
						return MMP_ERR_NONE;
					}
					if (kDwCnt <= STABLE_CNT) 
					{
						kUpCnt = 0;
						kDwCnt++;
					}
				}
			}

			*arg = GSNR_POSITION_NOCHANGE;
			return MMP_ERR_NONE;
		}
		break;

		default:
		break;
	}

	return status;
}

MMP_BOOL Gsensor_Attached(struct _3RD_PARTY_GSENSOR *pthis)
{
#if (GSENSOR_MODULE_ATT_COMPILER)
    #if(GSENSOR_CONNECT_ENABLE)
    return MMP_TRUE;
    #else
    return MMP_FALSE;
    #endif	
#elif (GSENSOR_MODULE_ATT_GPIO)
	MMP_UBYTE ubValue = MMP_FALSE;

	if(GSENSOR_MODULE_GPIO < MMP_GPIO_MAX)
	{
		MMPF_PIO_GetData(GSENSOR_MODULE_GPIO, &ubValue);
		return ubValue;
	}
	else
	{
		return AHC_FALSE;
	}
#endif

	return AHC_FALSE;
}

static struct _3RD_PARTY_GSENSOR Gsensor_Obj = 
{
    //0
    Gsensor_ResetDevice,
    Gsensor_ReadDeviceID,
    Gsensor_Initial,
    Gsensor_WriteRegister,
    Gsensor_ReadRegister,
	//Gsensor_ReadRegister_2byte,
	
    //5
    Gsensor_ReadOffsetFromFile,
    Gsensor_WriteOffsetToFile,
    Gsensor_GetSensorOffset,
    Gsensor_SetSensorOffset,
    Gsensor_Read_XYZ,
    
    //10
    Gsensor_Read_XYZ_WithCompensation,
    Gsensor_Read_Accel_Avg,
    Gsensor_CalculateOffset,
    Gsensor_Calibration,
    Gsensor_SetIntDuration,
    
    //15
    Gsensor_GetIntStatus,
    Gsensor_SetPowerSaveMode,
    Gsensor_SetNormalModeDataRate,
    Gsensor_SetInterruptSource,
    Gsensor_GetDataRate,
    
    //20
    Gsensor_SetDynamicRange,
    Gsensor_GetDynamicRange,
    Gsensor_SetGroundAxis,
    Gsensor_IOControl,
    Gsensor_Attached,
    
    //25
    Gsensor_Config_I2CM,
   
};

int Gsensor_Module_Init(void)
{
    printc(FG_GREEN("[GMA301 - Gsensor_Module_Init]\r\n"));
    MMPF_3RDParty_Gsensor_Register(&Gsensor_Obj);
    return 0;    
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6" 
#pragma O0
ait_module_init(Gsensor_Module_Init);
#pragma
#pragma arm section rodata, rwdata ,  zidata 

#endif		// ALL_FW

#ifdef MBOOT_FW
MMP_BOOL Gsensor_GetIntStatusForBoot(void)
{
	MMP_USHORT	value	= 0;

	MMPF_I2cm_ReadReg(&m_I2cmAttr_gsensor, (MMP_USHORT) 0x12, (MMP_USHORT*)&value);
	MMPF_I2cm_ReadReg(&m_I2cmAttr_gsensor, (MMP_USHORT) 0x13, (MMP_USHORT*)&value);

    if (value & 0x20)
        return MMP_TRUE;

    return MMP_FALSE;
}
#endif

#endif //GSENSOR_CONNECT_ENABLE && GSENSOR_MODULE == GSENSOR_MODULE_GMA301)


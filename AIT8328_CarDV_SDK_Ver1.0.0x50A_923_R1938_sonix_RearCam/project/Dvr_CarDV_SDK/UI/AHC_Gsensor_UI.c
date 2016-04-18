
//==============================================================================
//
//  File        : AHC_Gsensor_UI.c
//  Description : AHC Gsensor UI layer function
//  Author      : 
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Common.h"
#include "AHC_Menu.h"
#include "AHC_Message.h"
#include "AHC_Gsensor.h"
#include "AHC_General.h"
#include "AHC_General_CarDV.h"
#include "AHC_Parameter.h"
#include "dsc_key.h" 
#include "mmpf_gsensor.h"
#include "Gsensor_Ctrl.h"
#include "MenuSetting.h"
#include "StateVideoFunc.h"

#if (GSENSOR_CONNECT_ENABLE)

/*===========================================================================
 * Global varible
 *===========================================================================*/ 
#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
UINT16		TempGValue[3] = {0, 0, 0};
#endif
AHC_BOOL   	dump_GValues = AHC_FALSE;

AHC_BOOL 	m_ubGsnrIsObjMove 			= AHC_FALSE; //Status, check if the system is shaking or not.
AHC_BOOL	m_ubGsnrIsObjMove_Parking	= AHC_FALSE;
AHC_BOOL	m_ubGsnrEnter_Parking		= AHC_FALSE;
AHC_BOOL	m_ubGsnrExit_Parking        = AHC_FALSE;

#if (SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN)
extern UINT32       GSNRPowerOnThd;
extern AHC_BOOL     ubGsnrPwrOnActStart;
extern UINT32       m_ulGSNRRecStopCnt;
#endif

/*===========================================================================
 * Main body
 *===========================================================================*/
AHC_BOOL AHC_Gsensor_GetIntGpioStatus(MMP_UBYTE *pstatus)
{
	MMP_UBYTE 	value 	= 0;

#if POWER_ON_BY_GSENSOR_EN && defined(GSENSOR_INT)
    if (GSENSOR_INT != MMP_GPIO_MAX) {
        AHC_ConfigGPIOPad(GSENSOR_INT, 0);
    	AHC_ConfigGPIO(GSENSOR_INT, MMP_FALSE);
    	AHC_GetGPIO(GSENSOR_INT, &value);
    }
#endif

	*pstatus = (value & 0xFF);
	
	return MMP_ERR_NONE;
}

/* Use Gsensor Acceleration */
UINT32 AHC_Gsensor_GetAcceSensitivity(AHC_GSENSOR_COORDINATES coordinates)//Move to UI layer
{
    switch (coordinates) {
    case GSENSOR_XYZ:
        {
        	UINT32	thd = 0xFFFFFFFF;//Unit: gravivy tolerance * (Scale 100)

        	switch(MenuSettingConfig()->uiGsensorSensitivity)
            {
        	#if (MENU_GENERAL_GSENSOR_EN==GSENSOR_5_LEVEL)
        		#if (MENU_GENERAL_GSENSOR_LEVEL0_EN)
        		case GSENSOR_SENSITIVITY_L0:
        			thd = GSNR_SENS_5LEVEL0;
        		break;
        		#endif
        		#if (MENU_GENERAL_GSENSOR_LEVEL1_EN)
        		case GSENSOR_SENSITIVITY_L1:
        			thd = GSNR_SENS_5LEVEL1;
        		break;
        		#endif
        		#if (MENU_GENERAL_GSENSOR_LEVEL2_EN)
        		case GSENSOR_SENSITIVITY_L2:
        			thd = GSNR_SENS_5LEVEL2;
        		break;
        		#endif
        		#if (MENU_GENERAL_GSENSOR_LEVEL3_EN)
        		case GSENSOR_SENSITIVITY_L3:
        			thd = GSNR_SENS_5LEVEL3;
        		break;
        		#endif
        		#if (MENU_GENERAL_GSENSOR_LEVEL4_EN)
        		case GSENSOR_SENSITIVITY_L4:
        			thd = GSNR_SENS_5LEVEL4;
        		break;
        		#endif
        	#elif (MENU_GENERAL_GSENSOR_EN==GSENSOR_4_LEVEL)
        		case GSENSOR_SENSITIVITY_L0:	// HIGH
        			thd = GSNR_SENS_5LEVEL0;
        		break;
        		case GSENSOR_SENSITIVITY_L2:	// MIDDLE
        			thd = GSNR_SENS_5LEVEL2;
        		break;
        		case GSENSOR_SENSITIVITY_L3:	// LOW
        			thd = GSNR_SENS_5LEVEL3;
        		break;
        		case GSENSOR_SENSITIVITY_L4:	// STANDARD
        			thd = GSNR_SENS_5LEVEL4;
        		break;
        	#else
        		#if (MENU_GENERAL_GSENSOR_LEVEL0_EN)
        		case GSENSOR_SENSITIVITY_L0:
        			thd = GSNR_SENS_3LEVEL0;
        		break;
        		#endif
        		#if (MENU_GENERAL_GSENSOR_LEVEL2_EN)
        		case GSENSOR_SENSITIVITY_L2:
        			thd = GSNR_SENS_3LEVEL1;
        		break;
        		#endif
        		#if (MENU_GENERAL_GSENSOR_LEVEL4_EN)
        		case GSENSOR_SENSITIVITY_L4:
        			thd = GSNR_SENS_3LEVEL2;
        		break;
        		#endif
        	#endif
        	    default:
        	        thd = 0xFFFFFFFF;
        	    break;
            }

        	return thd;
        }
	    break;

    case GSENSOR_X:
    case GSENSOR_Y:
    case GSENSOR_Z:
        printc("--W-- %s - TBD\r\n", __func__);
    	return 0xFFFFFFFF;
    }

	return 0xFFFFFFFF;
}

UINT32 AHC_Gsensor_GetAcceSensitivity_Parking(AHC_GSENSOR_COORDINATES coordinates) //Move to Parking mode?
{
    printc("--W-- %s is TBD\r\n", __func__);
	return 0xFFFFFFFF;
}

#if (C005_PARKING_GSENSOR_ENTRY_CRITERIA == 1)
UINT32 AHC_Gsensor_GetSensitivityEntry_Parking(AHC_GSENSOR_COORDINATES coordinates) ////Move to Parking mode?
{
    printc("--W-- %s is TBD\r\n", __func__);
	return 0xFFFFFFFF;
}
#endif

AHC_BOOL AHC_Gsensor_GetCurAcceleration(AHC_BOOL* bObjMove, AHC_BOOL* bObjMove_1, AHC_BOOL* bObjMove_2)
{
	if(AHC_Gsensor_Module_Attached())
	{
		MMP_ERR 		err;
		AHC_GSENSOR_DATA 	sdata = {0, 0, 0};
		MMP_SHORT		xyz[3] = {0, 0, 0};
		UINT32			CurSensitivity = 0;
#if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
		UINT32			CurSensitivityParking = 0;
		#if(C005_PARKING_GSENSOR_ENTRY_CRITERIA)
		UINT32		CurEntryParking = 0;
		#endif
#endif
#if (MENU_MOVIE_GSENSOR_SENSITIVITY_XYZ_EN)
		UINT32			SensX = 0,SensY = 0,SensZ = 0;
#endif
#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
		static	UINT8   ubCounter = 10;//Check the different value every 1 second.
		static  MMP_SHORT		TempMax[3] = {0, 0, 0};
		MMP_UBYTE		i;
#endif

#if (MENU_MOVIE_GSENSOR_SENSITIVITY_XYZ_EN)
		SensX = AHC_Gsensor_GetAcceSensitivity(GSENSOR_X);
		SensY = AHC_Gsensor_GetAcceSensitivity(GSENSOR_Y);
		SensZ = AHC_Gsensor_GetAcceSensitivity(GSENSOR_Z);
#else
		CurSensitivity = AHC_Gsensor_GetAcceSensitivity(GSENSOR_XYZ);

		#if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
		CurSensitivityParking = AHC_Gsensor_GetAcceSensitivity_Parking(GSENSOR_XYZ);
		#if(C005_PARKING_GSENSOR_ENTRY_CRITERIA)
		CurEntryParking = AHC_Gsensor_GetSensitivityEntry_Parking(GSENSOR_XYZ);
		#endif
		#endif
#endif

		//printc("Sen Nor : %d\r\n",CurSensitivity);
		//printc("Sen Parking : %d\r\n",CurSensitivityParking);
#if (GSENSOR_MODULE==GSENSOR_MODULE_LIS3DH)	

		#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
			err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);
			xyz[0] = ((signed char)(sdata.acc_val[0]) *100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			xyz[1] = ((signed char)(sdata.acc_val[1]) *100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			xyz[2] = ((signed char)(sdata.acc_val[2]) *100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			
			if(xyz[0] > 200 || xyz[1] > 200 || xyz[2] > 200)
			{
    			if(dump_GValues)
                {
    				printc("xyz[0] : %d\r\n",xyz[0]);
    				printc("xyz[1] : %d\r\n",xyz[1]);
    				printc("xyz[2] : %d\r\n",xyz[2]);
                }
			}
		#else
			err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_XYZ, (MMP_UBYTE*) &sdata);
			xyz[0] = (abs((signed char)(sdata.acc_val[0])) *100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			xyz[1] = (abs((signed char)(sdata.acc_val[1])) *100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			xyz[2] = (abs((signed char)(sdata.acc_val[2])) *100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;

    		if(dump_GValues)
        		printc("G-XYZ: %03d %03d %03d\r\n", (signed short) xyz[0], (signed short) xyz[1], (signed short) xyz[2]);
		#endif

#elif (GSENSOR_MODULE == GSENSOR_MODULE_DMARD)

		err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);
		
		xyz[0] = (sdata.acc_val[0] *100) / GSENSOR_ACCELERATION_UNIT_DMARD06;
		xyz[1] = (sdata.acc_val[1] *100) / GSENSOR_ACCELERATION_UNIT_DMARD06;
		xyz[2] = (sdata.acc_val[2] *100) / GSENSOR_ACCELERATION_UNIT_DMARD06;

#elif ((GSENSOR_MODULE == GSENSOR_MODULE_GMA301) || (GSENSOR_MODULE == GSENSOR_MODULE_GMA302) || (GSENSOR_MODULE == GSENSOR_MODULE_GMA305))

		err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);
		xyz[0] = (abs((signed char)(sdata.acc_val[0])) * SCALE_G) / GSENSOR_ACCELERATION_UNIT_GMA301;
		xyz[1] = (abs((signed char)(sdata.acc_val[1])) * SCALE_G) / GSENSOR_ACCELERATION_UNIT_GMA301;
		xyz[2] = (abs((signed char)(sdata.acc_val[2])) * SCALE_G) / GSENSOR_ACCELERATION_UNIT_GMA301;

		if(dump_GValues)
    		printc("G-XYZ: %03d %03d %03d\r\n", (signed short) xyz[0], (signed short) xyz[1], (signed short) xyz[2]);
#elif (GSENSOR_MODULE == GSENSOR_MODULE_RT3000)
		err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_XYZ, (MMP_UBYTE*) &sdata);
		xyz[0] = (abs((signed char)(sdata.acc_val[0])) *100) / GSENSOR_ACCELERATION_UNIT_GMA301;
		xyz[1] = (abs((signed char)(sdata.acc_val[1])) *100) / GSENSOR_ACCELERATION_UNIT_GMA301;
		xyz[2] = (abs((signed char)(sdata.acc_val[2])) *100) / GSENSOR_ACCELERATION_UNIT_GMA301;

		if(dump_GValues)
			printc("G-XYZ: %03d %03d %03d\r\n", (signed short) xyz[0], (signed short) xyz[1], (signed short) xyz[2]);
#endif
		
#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
		ubCounter--;
		for(i = 0; i< 3; i++)
		{
			if(abs(xyz[i]) > abs(TempMax[i]))
			{
				TempMax[i] = abs(xyz[i]);
				TempGValue[i] = xyz[i];
			}
		}
		if(ubCounter == 0)
		{
			xyz[0] = TempMax[0];
			xyz[1] = TempMax[1];
			xyz[2] = TempMax[2];
			if(dump_GValues)
			{
				//printc("GValue[0] : %d\r\n",TempGValue[0]);
				//printc("GValue[1] : %d\r\n",TempGValue[1]);
				//printc("GValue[2] : %d\r\n",TempGValue[2]);
				
				printc("xyz[0] : %d\r\n",xyz[0]);
				printc("xyz[1] : %d\r\n",xyz[1]);
				printc("xyz[2] : %d\r\n",xyz[2]);
			}
		}
#endif
		
#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
		if(ubCounter == 0)
#endif
		{
			#if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
			if(	xyz[0] >= CurSensitivityParking || 
				xyz[1] >= CurSensitivityParking ||
				xyz[2] >= CurSensitivityParking)
			{
			    if (bObjMove_1)
				    *bObjMove_1 = AHC_TRUE;
			}
			else
			{
			    if (bObjMove_1)
				    *bObjMove_1 = AHC_FALSE;
			}
			#else
			{
			    if (bObjMove_1)
			        *bObjMove_1 = AHC_FALSE;
			}
			#endif
			
			#if(SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)
				#if (C005_PARKING_GSENSOR_ENTRY_CRITERIA == 1)
				if(	xyz[0] >= CurEntryParking || 
					xyz[1] >= CurEntryParking ||
					xyz[2] >= CurEntryParking)
				{
				    if (bObjMove_2)
					    *bObjMove_2 = AHC_TRUE;
				}
				else
				{
				    if (bObjMove_2)
					    *bObjMove_2 = AHC_FALSE;
			    }
				#else
				{
				    if (bObjMove_2)
				        *bObjMove_2 = AHC_FALSE;
				}
				#endif
			#endif
			
			#if (MENU_MOVIE_GSENSOR_SENSITIVITY_XYZ_EN)
			if(	xyz[0] >= SensX || 
				xyz[1] >= SensY ||
				xyz[2] >= SensZ)
		    {
			    if (bObjMove)
				    *bObjMove = AHC_TRUE;
			}
			else
			{
			    if (bObjMove)
				    *bObjMove = AHC_FALSE;
		    }
			#else

			if(	xyz[0] >= CurSensitivity || 
				xyz[1] >= CurSensitivity ||
				xyz[2] >= CurSensitivity)
			{
			    if (bObjMove)
				    *bObjMove = AHC_TRUE;
			}
			else
			{
			    if (bObjMove)
				    *bObjMove = AHC_FALSE;
			}
			#endif

			if( xyz[0] >= 15 || 
				xyz[1] >= 15 ||
				xyz[2] >= 15)
			{
				m_ubGsnrExit_Parking = AHC_TRUE;
			}
			else
			{
				m_ubGsnrExit_Parking = AHC_FALSE;
			}
			
			#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
			for(i = 0; i< 3; i++)
			{
				TempMax[i] = 0;	
			}

			ubCounter = 10;	
			#endif
		}
		#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
		else
		{
		    if (bObjMove)
			    *bObjMove = AHC_FALSE;
		}
		#endif

	}
	else
	{
	    if (bObjMove)
		    *bObjMove = AHC_FALSE;
	}
	return AHC_TRUE;
}

MMP_UBYTE AHC_Gsensor_IsGroundAxisRotate(void)
{
	if(AHC_Gsensor_Module_Attached())
	{
		MMP_UBYTE		pos;

		AHC_Gsensor_IOControl(GSNR_CMD_GROUND_AXIS_STATUS, (void *) &pos);
	
    	return pos;
	}

	return GSNR_POSITION_NOCHANGE;
}

/* Use Gsensor Interrupt Status */
AHC_BOOL AHC_Gsensor_SetIntThreshold(void)
{
	if(AHC_Gsensor_Module_Attached())
	{
		MMP_ERR 	err;
		int 		thd;

        thd = AHC_Gsensor_GetAcceSensitivity(GSENSOR_XYZ);
 
     	err = AHC_Gsensor_IOControl(GSNR_CMD_SET_INT_THD , (void *) &thd);
	}

	return AHC_TRUE;
}

AHC_BOOL AHC_Gsensor_GetCurIntStatus(AHC_BOOL* bObjMove)
{
	if (AHC_Gsensor_Module_Attached())
	{
		MMP_ERR 		err;
		MMP_UBYTE		int_status;
		
		err = AHC_Gsensor_IOControl(GSNR_CMD_GET_INT_STATUS, &int_status);
		
		if(int_status)
			*bObjMove = AHC_TRUE;
		else
			*bObjMove = AHC_FALSE;
	}
	
	return AHC_TRUE;
}

AHC_BOOL AHC_Gsensor_Polling(UINT8 ubType)
{
	if(AHC_Gsensor_Module_Attached())
	{
        AHC_BOOL bLockVRFile = VideoFunc_LockFileEnabled();

		switch(ubType)
		{
		case MOVE_BY_ACCELERATION:
		    #if (POWER_ON_BY_GSENSOR_EN)
			if(	MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF && 
				!bLockVRFile && !m_ubGsnrIsObjMove)
			{
				extern AHC_BOOL ubUIInitDone(void);
				// when m_ubGsnrIsObjMove was TRUE, should not call it,
				// must wait m_ubGsnrIsObjMove cleaned!
				
				if(ubGsnrPwrOnActStart && ubUIInitDone())
				{
					AHC_Gsensor_GetCurAcceleration(&m_ubGsnrIsObjMove, &m_ubGsnrIsObjMove_Parking, &m_ubGsnrEnter_Parking);
					
					if(m_ubGsnrIsObjMove)
					{
						if(GSNR_PWRON_REC_BY && GSNR_PWRON_REC_BY_SHAKED)
						{
							m_ulGSNRRecStopCnt = AHC_GSNR_PWROn_MovieTimeReset();
						
							if(!VideoFunc_RecordStatus())
								AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_VIDEO_RECORD, 0);
						}
					}
				}
				else
				{
					if(VideoFunc_RecordStatus())
					    AHC_Gsensor_GetCurAcceleration(&m_ubGsnrIsObjMove, &m_ubGsnrIsObjMove_Parking, &m_ubGsnrEnter_Parking);
				}
			}
			else
			{
				if(ubGsnrPwrOnActStart && ubUIInitDone())
				{
					AHC_BOOL ubShaked;
				    AHC_Gsensor_GetCurAcceleration(&ubShaked, NULL, NULL);
					
					if(ubShaked)
					{
						if(GSNR_PWRON_REC_BY && GSNR_PWRON_REC_BY_SHAKED)
						{
							m_ulGSNRRecStopCnt = AHC_GSNR_PWROn_MovieTimeReset();
						}
					}
				}
			}
			#else
			if(	MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF && 
				VideoFunc_RecordStatus() && !bLockVRFile && !m_ubGsnrIsObjMove)
			{	
				AHC_Gsensor_GetCurAcceleration(&m_ubGsnrIsObjMove, &m_ubGsnrIsObjMove_Parking, &m_ubGsnrEnter_Parking);
			}
            #endif
		    break;

		case MOVE_BY_INTERUPT:
			if (dump_GValues)
			{
				AHC_GSENSOR_DATA 	sdata;

				AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);
				printc("%03d %03d %03d\r\n", (signed char)sdata.acc_val[0], (signed char)sdata.acc_val[1], (signed char)sdata.acc_val[2]);	
			}
            
		    #if (POWER_ON_BY_GSENSOR_EN)
			if(	MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF && 
				!bLockVRFile && !m_ubGsnrIsObjMove)
			{
				// when m_ubGsnrIsObjMove was TRUE, should not call it,
				// must wait m_ubGsnrIsObjMove cleaned!
				
				if(ubGsnrPwrOnActStart && ubUIInitDone())
				{
					AHC_Gsensor_GetCurIntStatus(&m_ubGsnrIsObjMove);
					
					if(m_ubGsnrIsObjMove)
					{
						if(GSNR_PWRON_REC_BY && GSNR_PWRON_REC_BY_SHAKED)
						{
							m_ulGSNRRecStopCnt = AHC_GSNR_PWROn_MovieTimeReset();
						
							if(!VideoFunc_RecordStatus())
								AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, KEY_VIDEO_RECORD, 0);
						}
					}
				}
				else
				{
					if(VideoFunc_RecordStatus())
						AHC_Gsensor_GetCurIntStatus(&m_ubGsnrIsObjMove);
				}
			}
			else
			{
				if(ubGsnrPwrOnActStart && ubUIInitDone())
				{
					AHC_BOOL ubShaked;
					AHC_Gsensor_GetCurIntStatus(&ubShaked);
					
					if(ubShaked)
					{
						if(GSNR_PWRON_REC_BY && GSNR_PWRON_REC_BY_SHAKED)
						{
							m_ulGSNRRecStopCnt = AHC_GSNR_PWROn_MovieTimeReset();
						}
					}
				}
			}
			#else
			if(	MenuSettingConfig()->uiGsensorSensitivity != GSENSOR_SENSITIVITY_OFF && 
				VideoFunc_RecordStatus() && !bLockVRFile && !m_ubGsnrIsObjMove)
			{
				// when m_ubGsnrIsObjMove was TRUE, should not call it,
				// must wait m_ubGsnrIsObjMove cleaned!
				AHC_Gsensor_GetCurIntStatus(&m_ubGsnrIsObjMove);
			}
			#endif
			break;

		case Z_AXIS_GRAVITY_CAHNGE:
		{
		    static MMP_UBYTE 	ubDetectCnt = 0;
		    MMP_UBYTE			pos;
		
    		ubDetectCnt = (ubDetectCnt+1) % 8;
    
    		if(ubDetectCnt>0)
        		break;

			AHC_SetRotateSrc(SRC_G_SNR);
			
			pos = AHC_Gsensor_IsGroundAxisRotate();

#if (FLIP_CTRL_METHOD & CTRL_BY_G_SNR) && (G_SNR_FLIP_SELECT != FLIP_NONE) 
			if (pos == GSNR_POSITION_DOWN) {
				AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, DEVICE_LCD_INV, 0);
			} else if(pos == GSNR_POSITION_UP) {
				AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, DEVICE_LCD_NOR, 0);
			}
#endif
		}
			break;
			
		default:
			// NOP
			break;
		}
		
		if(m_ubGsnrIsObjMove || m_ubGsnrIsObjMove_Parking)
		{
			#if ((SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_1)||((SUPPORT_PARKINGMODE == PARKINGMODE_STYLE_2)))
			if(uiGetParkingModeEnable() && m_ubGsnrIsObjMove_Parking)			
			{
				if(!VideoFunc_RecordStatus() && (uiGetCurrentState() != UI_PARKING_MENU_STATE && uiGetCurrentState() != UI_PARKING_EXIT_STATE 
				&& uiGetCurrentState() != UI_PARKING_MOVE_HINT && uiGetCurrentState() != UI_PARKING_SPACE_HINT))
				{
#if GSENSOR_DBG_MSG
					printc("Parking mode Gsensor Act\r\n");

                    #if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
					printc("GValue 0 %d\r\n",TempGValue[0]);
					printc("GValue 1 %d\r\n",TempGValue[1]);
					printc("GValue 2 %d\r\n",TempGValue[2]);
                    #endif
#endif

					uiSetParkingModeRecordState(AHC_TRUE);
					uiSetParkingModeRecord(AHC_TRUE);
					//SetKeyPadEvent(BUTTON_SET_PRESS);
					GPSCtrl_GPSRawResetBuffer();
					AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_RECSTART_PRESS, 0);
				}
			}
			else if(!uiGetParkingModeEnable())
			#endif
			{
				if(m_ubGsnrIsObjMove)
				{
                    if (AHC_SDMMC_BasicCheck()) {
#if GSENSOR_DBG_MSG
    					printc("Normal mode Gsensor Act\r\n");

                        #if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
    					printc("GValue 0 %d\r\n",TempGValue[0]);
    					printc("GValue 1 %d\r\n",TempGValue[1]);
    					printc("GValue 2 %d\r\n",TempGValue[2]);
    					#endif
#endif
                    
    					AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, GSENSOR_ACT, 0);
    					AHC_OS_SleepMs(100);
                    }
				}
			}
			
		}
	}
    
	return AHC_TRUE;
}

#endif

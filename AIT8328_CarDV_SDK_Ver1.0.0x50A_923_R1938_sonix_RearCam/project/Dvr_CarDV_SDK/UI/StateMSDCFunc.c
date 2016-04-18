/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "customer_config.h"
#include "AHC_Common.h"
#include "AHC_General.h"
#include "AHC_Menu.h"
#include "AHC_Media.h"
#include "AHC_Parameter.h"
#include "AHC_Usb.h"
#include "AHC_Warningmsg.h"
#include "AHC_Os.h"
#include "AHC_Message.h"
#include "AHC_General_CarDV.h"
#include "AHC_Display.h"
#include "KeyParser.h"
#include "StateMSDCFunc.h"
#include "MenuSetting.h"
#include "LedControl.h"
#include "dsc_charger.h"
#include "DrawStateMSDCFunc.h"
#include "mmps_usb.h"

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

static AHC_BOOL bUsbMsdcMode 		= AHC_FALSE;
static UINT32   MSDCCounter			= 0;
static UINT8    MSDCPowerOffCounter = 0;
UINT8  			MSDCTimerID			= 0;
UINT8			ubUSBLedStatus		= AHC_TRUE;
UINT8			gi=0;

static AHC_BOOL bUsbPCCAMMode 		= AHC_FALSE;

/*===========================================================================
 * Local function
 *===========================================================================*/ 

#ifdef	MSDC_STATUS_LED
static void MsdcLedUpdate(void);
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/

extern AHC_BOOL bForce_PowerOff;
extern AHC_BOOL bChargeFull;
/*===========================================================================
 * Main body
 *===========================================================================*/ 

void MSDCTimer_ResetCounter(void)
{
    MSDCCounter = 0;
}

void MSDCTimer_ISR(void *tmr, void *arg)
{  
    MSDCCounter++;
    
    if( MSDCCounter & 0x01)
        AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
}

AHC_BOOL MSDCTimer_Start(UINT32 ulTime)
{
    MSDCTimer_ResetCounter();
    
	MSDCTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, MSDCTimer_ISR, (void*)NULL );
    
    if(0xFE <= MSDCTimerID)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

AHC_BOOL MSDCTimer_Stop(void)
{
    UINT8 ret = 0;
    
	ret = AHC_OS_StopTimer(MSDCTimerID, OS_TMR_OPT_NONE);
  
	if(0xFF == ret)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

AHC_BOOL MSDCFunc_Status(void)
{
    return bUsbMsdcMode;
}

AHC_BOOL MSDCFunc_Enable(AHC_BOOL bEnable)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    if( bEnable )
    {
        if( !bUsbMsdcMode )
        {
            bUsbMsdcMode = AHC_TRUE;
            ahc_ret = AHC_SetMode(AHC_MODE_USB_MASS_STORAGE);
        }
    }
    else
    {
        if( bUsbMsdcMode )
        {
            bUsbMsdcMode = AHC_FALSE;
            ahc_ret = AHC_SetMode(AHC_MODE_IDLE);
        }
    }

    return ahc_ret;
}

AHC_BOOL PCCAMFunc_Enable(AHC_BOOL bEnable)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    if( bEnable )
    {
        if( !bUsbPCCAMMode )
        {
            bUsbPCCAMMode = AHC_TRUE;
            ahc_ret = AHC_SetMode(AHC_MODE_USB_WEBCAM);
        }
    }
    else
    {
        if( bUsbPCCAMMode )
        {
            bUsbPCCAMMode = AHC_FALSE;
            ahc_ret = AHC_SetMode(AHC_MODE_IDLE);
        }
    }

    return ahc_ret;
}

void MSDC_Func_CheckLCDstatus(void)
{
	UINT8 status;

	if ((AHC_FALSE == AHC_IsTVConnectEx()) && (AHC_FALSE == AHC_IsHdmiConnect()))
    {
    	AHC_LCD_GetStatus(&status);
    		
    	AHC_SetKitDirection(status, AHC_TRUE, AHC_SNR_NOFLIP, AHC_FALSE);
    }
}

AHC_BOOL MSDCMode_Start(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    ahc_ret = MSDCFunc_Enable(AHC_TRUE);
    
	MSDC_Func_CheckLCDstatus();
	MSDCTimer_ResetCounter();
	MSDCTimer_Start(100);
		
	if(AHC_TRUE == ahc_ret )
    {
        DrawStateMSDCUpdate( EVENT_USB_DETECT );

#ifdef CFG_MSDC_BYPASS_CHANGE_ICON //may be defined in config_xxx.h
		#if	OSD_SHOW_BATTERY_STATUS == 1
        AHC_EnableChargingIcon(AHC_TRUE);
		#endif
#endif
    }

    return ahc_ret;
}

AHC_BOOL PCCAMMode_Start(void)
{
    AHC_BOOL ahc_ret = AHC_TRUE;

    ahc_ret = PCCAMFunc_Enable(AHC_TRUE);

    MSDC_Func_CheckLCDstatus();
    MSDCTimer_ResetCounter();
    MSDCTimer_Start(100);
		
    if(AHC_TRUE == ahc_ret )
    {
        printc("%s share with MSDC draw \r\n", __func__);
        DrawStateMSDCUpdate( EVENT_USB_DETECT );

#ifdef CFG_MSDC_BYPASS_CHANGE_ICON //may be defined in config_xxx.h
		#if	OSD_SHOW_BATTERY_STATUS == 1
        AHC_EnableChargingIcon(AHC_TRUE);
		#endif
#endif
    }

    return ahc_ret;
}

//******************************************************************************
//
//                              AHC State MSDC Mode
//
//******************************************************************************

void StateMSDCMode( UINT32 ulEvent )
{       
    switch( ulEvent )
    {
        case EVENT_NONE                           :
        break;
		
		case EVENT_POWER_OFF                      :
			AHC_NormalPowerOffPath();
        break;
		
		//Device
        case EVENT_USB_DETECT                     :
        break;
	#ifdef NET_SYNC_PLAYBACK_MODE
		case EVENT_NET_ENTER_PLAYBACK :
			if(AHC_USB_GetLastStatus() == AHC_USB_NORMAL) // MSDC/PCCAM mode
            {
                AHC_USB_OP_MODE uiMode;
                AHC_GetUsbMode(&uiMode);

                bUsbMsdcMode = AHC_FALSE;
                MSDCTimer_Stop();
                AHC_DisconnectDevice();
                if (uiMode == AHC_USB_MODE_MASS_STORAGE) {
                    RTNA_DBG_Str(0, "AHC_USB_MODE_MASS_STORAGE\r\n");
                    AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
                    StateSwitchMode(UI_NET_PLAYBACK_STATE);

                    // Unlock HDMI/TV state when USB plug-out
                    AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
                    AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
                }
                else
                if (uiMode == AHC_USB_MODE_WEBCAM) {
                    RTNA_DBG_Str(0, "AHC_USB_MODE_WEBCAM\r\n");
                    PCCAMFunc_Enable(AHC_FALSE);
                    StateSwitchMode(UI_NET_PLAYBACK_STATE);    
                }
                else
                {
                	StateSwitchMode(UI_NET_PLAYBACK_STATE);   
                }
                // Unlock HDMI/TV state when USB plug-out
                AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
                AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
			}
	
            CGI_FEEDBACK(ulEvent, 0 /* SUCCESSFULLY */);
		break;
	#endif
		case EVENT_USB_REMOVED                    :
            if(AHC_USB_GetLastStatus() == AHC_USB_NORMAL) // MSDC/PCCAM mode
            {
                AHC_USB_OP_MODE uiMode;
                AHC_GetUsbMode(&uiMode);

                bUsbMsdcMode = AHC_FALSE;
                MSDCTimer_Stop();
                AHC_DisconnectDevice();

#ifdef	MSDC_OUT_NOT_OFF
                if (uiMode == AHC_USB_MODE_MASS_STORAGE) {
                    RTNA_DBG_Str(0, "AHC_USB_MODE_MASS_STORAGE\r\n");
                    AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
                    StateSwitchMode(UI_VIDEO_STATE);

                    // Unlock HDMI/TV state when USB plug-out
                    AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
                    AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
                }
                else
#endif
#ifdef PCCAM_OUT_NOT_OFF 
                if (uiMode == AHC_USB_MODE_WEBCAM) {
                    RTNA_DBG_Str(0, "AHC_USB_MODE_WEBCAM\r\n");
                    PCCAMFunc_Enable(AHC_FALSE);
                    StateSwitchMode(UI_VIDEO_STATE);

                    // Unlock HDMI/TV state when USB plug-out
                    AHC_LockHdmiConnectionStatus(AHC_FALSE, AHC_FALSE);
                    AHC_LockTVConnectionStatus(AHC_FALSE, AHC_FALSE);
                }
                else
#endif
                {
                    AHC_SetMode(AHC_MODE_IDLE);
                    bForce_PowerOff = AHC_TRUE;
                    AHC_NormalPowerOffPath();
                }
            }

            if (AHC_IsDcCableConnect() == AHC_FALSE)
                AHC_SetChargerEnable(AHC_FALSE);
            break;

        case EVENT_SD_DETECT                      :
			AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
        break;

        case EVENT_SD_REMOVED                     :
            if(AHC_TRUE == AHC_SDMMC_GetMountState()) {
                AHC_DisMountStorageMedia(AHC_MEDIA_MMC);
				Enable_SD_Power(0 /* Power Off */);
            }
        break;
        
        case EVENT_LCD_COVER_OPEN                  :
			LedCtrl_LcdBackLight(AHC_TRUE);
        break;
        
        case EVENT_LCD_COVER_CLOSE                :
			LedCtrl_LcdBackLight(AHC_FALSE);
        break;    
            
        case EVENT_LCD_COVER_NORMAL               :
        case EVENT_LCD_COVER_ROTATE               :
        	AHC_DrawRotateEvent(ulEvent);  	
        break;

        case EVENT_MSDC_UPDATE_MESSAGE:
			if(!AHC_IsUsbConnect())
			{        	
				if(MSDCPowerOffCounter == MSDC_POWEROFF_COUNT)
				{
					printc("--I-- exceed to MSDC_POWEROFF_COUNT, going to power off\r\n");
					
					#ifdef MSDC_OUT_NOT_OFF
					// Not to Power off after MSDC out
					#else
					bForce_PowerOff = AHC_TRUE;	
					AHC_NormalPowerOffPath();
					#endif
				}

				MSDCPowerOffCounter++;
			}
			else
			{
				#ifdef	MSDC_STATUS_LED
	        	MsdcLedUpdate();
				#endif

				MSDCPowerOffCounter = 0;
			}
        break;
       		
        default:
        break;
    }
}

#ifdef	MSDC_STATUS_LED

static int _ledblk = 0;
static int _ledper = 0;
static int _ledcnt = 0;

void _uiMsdcLed(int rw, int err)
{
	if (err == 0) {
		_ledblk = 1;
		_ledper = 5;
	}
}

void MsdcLedUpdate(void)
{
	if (_ledcnt > 0) {
		// MSDC is reading or writing 
		LedCtrl_ButtonLed(MSDC_READ_LED, (_ledcnt & 1));
		_ledcnt--;
	} else {
		if (_ledblk) {
			// MSDC start to read or write
			_ledblk = 0;
			_ledcnt = _ledper;
		}
		LedCtrl_ButtonLed(MSDC_READ_LED, AHC_FALSE);
	}
}
#endif
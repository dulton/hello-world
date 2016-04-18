/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "customer_config.h" 
#include "AHC_Common.h"
#include "AHC_Parameter.h"
#include "AHC_General.h"
#include "AHC_General_CarDV.h"
#include "AHC_Message.h"
#include "AHC_Utility.h"
#include "AHC_Display.h"
#include "AHC_Menu.h"
#include "AHC_Os.h"
#include "LedControl.h"
#include "ISP_if.h"
#if(BUTTON_BIND_LED)
#include "StateVideoFunc.h"
#endif
#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#include "AHC_Media.h"
#include "mmp_reg_display.h"
#include "lib_retina.h"

#if defined(WIFI_PORT) && (WIFI_PORT == 1)
#include "netapp.h"
#include "wlan.h"
#endif

/*===========================================================================
 * Macro define
 *===========================================================================*/ 

#define FLASH_LED_OFF 		(0)
#define FLASH_LED_ON		(1)
#define FLASH_LED_EV_TH_H	(650)//TBD
#define FLASH_LED_EV_TH_L	(220)//TBD

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

static AHC_BOOL bBackLightStatus;

static UINT8  	FlashLEDTimerID = 0xFF;
static UINT32  	FlashLEDCounter = 0;
static UINT8	FlashLEDStatus  = FLASH_LED_OFF;
static UINT8 	RecLEDTimerID	= 0xFF;
static UINT32  	RecLEDCounter = 0;
#ifdef LED_GPIO_FW_UPDATE
static UINT8	FwUpdateLEDTimerID = 0xFF;
static UINT32	FwUpdateLEDCounter = 0;
#endif
#if defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(LED_GPIO_WIFI_STATE)
static UINT8 	StreamingLEDTimerID	= 0xFF;
static UINT32  	StreamingLEDCounter = 0;
#endif

/*===========================================================================
 * Extern function
 *===========================================================================*/ 

extern MMP_ULONG AHC_GetVideoModeLedGPIOpin(void);
extern MMP_ULONG AHC_GetCameraModeLedGPIOpin(void);
extern MMP_ULONG AHC_GetPBModeLedGPIOpin(void);
extern MMP_ULONG AHC_GetSelfTimerLedGPIOpin(void);
extern MMP_ULONG AHC_GetAFLedGPIOpin(void);
extern MMP_ULONG AHC_GetLCDBacklightGPIOpin(void);
extern AHC_BOOL	bMuteRecord;

/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
void _____Flash_LED_Function_________(){ruturn;} //dummy
#endif

UINT32 FlashLED_GetEV(void)
{
	UINT32 ulLightLux = 0;
	
	ulLightLux = ISP_IF_AE_GetLightCond();
	
	return ulLightLux;
}

static void FlashLED_Timer_ISR(void *tmr, void *arg)
{   
    FlashLEDCounter++;
	
    if(0 == FlashLEDCounter%15) 
    {    
        if(FlashLEDStatus==FLASH_LED_ON && FlashLED_GetEV()>=FLASH_LED_EV_TH_H)
        	LedCtrl_FlashLed(AHC_FALSE);
        else if(FlashLEDStatus==FLASH_LED_ON && FlashLED_GetEV()<FLASH_LED_EV_TH_L)
        	;//Do nothing
        else if(FlashLEDStatus==FLASH_LED_OFF && FlashLED_GetEV()>=FLASH_LED_EV_TH_H)
        	;//Do nothing
        else if(FlashLEDStatus==FLASH_LED_OFF && FlashLED_GetEV()<FLASH_LED_EV_TH_L)
        	LedCtrl_FlashLed(AHC_TRUE);    	
    }         
}

AHC_BOOL FlashLED_Timer_Start(UINT32 ulTime)
{
    if(FlashLEDTimerID == 0xFF) 
    {
	    FlashLEDCounter = 0;
	    FlashLEDTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, FlashLED_Timer_ISR, (void*)NULL );

	    if(0xFE <= FlashLEDTimerID)
			return AHC_FALSE;
		else
			return AHC_TRUE;
	}	
	return AHC_FALSE;		
}

AHC_BOOL FlashLED_Timer_Stop(void)
{
    UINT8 ret = 0;
	
	if(FlashLEDTimerID < 0xFE)
	{
	    ret = AHC_OS_StopTimer( FlashLEDTimerID, AHC_OS_TMR_OPT_PERIODIC );

		FlashLEDCounter = 0;
		FlashLEDTimerID = 0xFF;

	    if(0xFF == ret)
	 		return AHC_FALSE;
		else
			return AHC_TRUE;
	}
	return AHC_FALSE;	
}

static void RecLED_Timer_ISR(void *tmr, void *arg)
{   
#if(BUTTON_BIND_LED)
	static UINT8	ubRecLedStatus		=	1;
	
	// Default
    if(VideoFunc_RecordStatus())//REC and MIC LED
	{
        LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), ubRecLedStatus & 0x08);
        ubRecLedStatus++;
    }
    else
    {
    	ubRecLedStatus = AHC_FALSE;

		#ifdef CFG_SDMMC_ERROR_TURNON_REC_LED
		if(AHC_FALSE == AHC_SDMMC_GetMountState())
		{
			LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_TRUE);
		}
		else
		#endif
		{
			LedCtrl_ButtonLed(AHC_GetVideoModeLedGPIOpin(), AHC_FALSE);
		}
	}


#endif//BUTTON_BIND_LED
}

AHC_BOOL RecLED_Timer_Start(UINT32 ulTime)
{
    if(RecLEDTimerID >= 0xFE) 
    {
	    RecLEDCounter = 0;
	    RecLEDTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, RecLED_Timer_ISR, (void*)NULL );

	    if(0xFE <=RecLEDTimerID)
			return AHC_FALSE;
		else
			return AHC_TRUE;
	}	
	
	return AHC_FALSE;
}

AHC_BOOL RecLED_Timer_Stop(void)
{
    UINT8 ret = 0;

	#ifdef CFG_REC_STATUS_INDEX
	if (CFG_REC_STATUS_INDEX != MMP_GPIO_MAX)
		AHC_SetGPIO(CFG_REC_STATUS_INDEX, 0);
	#endif

	if(RecLEDTimerID < 0xFE)
	{
	    ret = AHC_OS_StopTimer( RecLEDTimerID, AHC_OS_TMR_OPT_PERIODIC );

		RecLEDCounter = 0;
		RecLEDTimerID = 0xFF;

	    if(0xFF == ret)
	 		return AHC_FALSE;
		else
			return AHC_TRUE;
	}
    
	return AHC_FALSE;	
}

// For SD Card FW Update
#ifdef LED_GPIO_FW_UPDATE
static void FwUpdateLED_Timer_ISR(void *tmr, void *arg)
{
	static UINT8	ubUpdateLedStatus		=	1;

	LedCtrl_ButtonLed(LED_GPIO_FW_UPDATE, ubUpdateLedStatus);
	ubUpdateLedStatus ^= 1;
}
#endif

AHC_BOOL FwUpdateLED_Timer_Start(UINT32 ulTime)
{
#ifdef LED_GPIO_FW_UPDATE 
	if (LED_GPIO_FW_UPDATE == MMP_GPIO_MAX)
		return AHC_FALSE;

    if(FwUpdateLEDTimerID >= 0xFE)
    {
	    FwUpdateLEDCounter = 0;
	    FwUpdateLEDTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, FwUpdateLED_Timer_ISR, (void*)NULL );

	    if (0xFE <= FwUpdateLEDTimerID)
			return AHC_FALSE;
		else
			return AHC_TRUE;
	}
#endif

	return AHC_FALSE;
}

AHC_BOOL FwUpdateLED_Timer_Stop(void)
{
#ifdef LED_GPIO_FW_UPDATE 
    UINT8 ret = 0;

	if (FwUpdateLEDTimerID < 0xFE)
	{
	    ret = AHC_OS_StopTimer( FwUpdateLEDTimerID, AHC_OS_TMR_OPT_PERIODIC );

		FwUpdateLEDCounter = 0;
		FwUpdateLEDTimerID = 0xFF;

	    if(0xFF == ret)
	 		return AHC_FALSE;
		else
			return AHC_TRUE;
	}
#endif

	return AHC_FALSE;
}

#if defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(LED_GPIO_WIFI_STATE)
static void StreamingLED_Timer_ISR(void *tmr, void *arg)
{   
#ifdef CUS_STREAMING_LED_FUNC

	CUS_STREAMING_LED_FUNC();

#else

	static UINT8 ubStreamingLedStatus = AHC_TRUE;
    INT32 appStatus = get_NetAppStatus();

	if (WLAN_SYS_GetMode() == -1)
	{	
        LedCtrl_WiFiLed(AHC_FALSE);
        ubStreamingLedStatus = 0;
    }
    else
    {
        switch (appStatus) {
        case NETAPP_STREAM_PLAY:
            LedCtrl_WiFiLed(ubStreamingLedStatus & 0x08);
            ubStreamingLedStatus++;
            break;

        case NETAPP_STREAM_TEARDOWN:
        case NETAPP_WIFI_READY:
            LedCtrl_WiFiLed(AHC_TRUE);
            ubStreamingLedStatus = 0;
            break;

        case NETAPP_WIFI_IDLE:
            ubStreamingLedStatus = 0;

        	if (WLAN_SYS_GetMode() != -1) {
        		// Wi-Fi turn-on but no client
	            LedCtrl_WiFiLed(AHC_TRUE);
	            break;
        	}
			
            LedCtrl_WiFiLed(AHC_FALSE);
            break;
                    
        default:
            if (IS_WIFI_FAIL(appStatus)) {
                LedCtrl_WiFiLed(ubStreamingLedStatus & 0x04);
                ubStreamingLedStatus++;
            }
            else {
                LedCtrl_WiFiLed(AHC_FALSE);
                ubStreamingLedStatus = 0;
            }
        }
    }

#endif		// #ifdef CUS_STREAMING_LED_FUNC
}
#endif

AHC_BOOL WiFiStreamingLED_Timer_Start(UINT32 ulTime)
{
#if defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(LED_GPIO_WIFI_STATE)
	if (LED_GPIO_WIFI_STATE != MMP_GPIO_MAX)
	{
	    if (0xFE <= StreamingLEDTimerID)
	    {
		    StreamingLEDCounter = 0;
		    StreamingLEDTimerID = AHC_OS_StartTimer( ulTime, AHC_OS_TMR_OPT_PERIODIC, StreamingLED_Timer_ISR, (void *) NULL );

		    if (0xFE <= StreamingLEDTimerID)
				return AHC_FALSE;
			else
				return AHC_TRUE;
		}
	}
#endif
    
	return AHC_FALSE;
}

AHC_BOOL WiFiStreamingLED_Timer_Stop(void)
{
#if defined(WIFI_PORT) && (WIFI_PORT == 1) && defined(LED_GPIO_WIFI_STATE)
    if (LED_GPIO_WIFI_STATE != MMP_GPIO_MAX)
    {
        if (0xFE > StreamingLEDTimerID)
        {
            UINT8 ret = 0;

            ret = AHC_OS_StopTimer( StreamingLEDTimerID, AHC_OS_TMR_OPT_PERIODIC );

            StreamingLEDCounter = 0;
            StreamingLEDTimerID = 0xFF;

            if (0xFF == ret)
                return AHC_FALSE;
        }
    }
#endif
    
	return AHC_FALSE;
}

void LedCtrl_FlashLed(AHC_BOOL bOn)
{
#ifdef	LED_GPIO_FLASH_LIGHT
    AHC_BOOL bOutput = ((LED_GPIO_FLASH_LIGHT_ACT_LEVEL == GPIO_HIGH) ? GPIO_LOW : GPIO_HIGH);
    
    if (bOn)
        bOutput = LED_GPIO_FLASH_LIGHT_ACT_LEVEL;
        
    FlashLEDStatus = bOutput;

    AHC_ConfigGPIOPad(LED_GPIO_FLASH_LIGHT, PAD_OUT_DRIVING(0));
	AHC_ConfigGPIO(LED_GPIO_FLASH_LIGHT, AHC_TRUE);
	AHC_SetGPIO(LED_GPIO_FLASH_LIGHT, bOutput);
#endif
}
   
UINT8 LedCtrl_GetFlashLEDStatus(void)
{
	return FlashLEDStatus;
}

#if 0
void _____Other_LED_Function_________(){ruturn;} //dummy
#endif
   
void LedCtrl_VideoModeLed(AHC_BOOL bOn)
{
	if(AHC_GetVideoModeLedGPIOpin() != MMP_GPIO_MAX)
    {
	    AHC_BOOL bOutput;
	    
    	bOutput = (bOn)?(1):(0);

        AHC_ConfigGPIOPad(AHC_GetVideoModeLedGPIOpin(), PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(AHC_GetVideoModeLedGPIOpin(), AHC_TRUE);
		AHC_SetGPIO(AHC_GetVideoModeLedGPIOpin(), bOutput);
	}
}

void LedCtrl_CameraModeLed(AHC_BOOL bOn)
{
	if(AHC_GetCameraModeLedGPIOpin() != MMP_GPIO_MAX)
    {
	    AHC_BOOL bOutput;
	    
    	bOutput = (bOn)?(1):(0);

        AHC_ConfigGPIOPad(AHC_GetCameraModeLedGPIOpin(), PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(AHC_GetCameraModeLedGPIOpin(), AHC_TRUE);
		AHC_SetGPIO(AHC_GetCameraModeLedGPIOpin(), bOutput);
	}
}

void LedCtrl_PlaybackModeLed(AHC_BOOL bOn)
{  
    if(AHC_GetPBModeLedGPIOpin() != 0xFFF) //0xFFF = PIO_REG_UNKNOWN 
    {
	    AHC_BOOL bOutput;
	    
    	bOutput = (bOn)?(1):(0);

        AHC_ConfigGPIOPad(AHC_GetPBModeLedGPIOpin(), PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(AHC_GetPBModeLedGPIOpin(), AHC_TRUE);
		AHC_SetGPIO(AHC_GetPBModeLedGPIOpin(), bOutput);
	}
}

void LedCtrl_PowerLed(AHC_BOOL bOn)
{  
#if defined(LED_GPIO_POWER)
	if (LED_GPIO_POWER != MMP_GPIO_MAX)
	{
	    AHC_BOOL bOutput;

	    #if defined(LED_GPIO_POWER_ACT_LEVEL) //may be defined in config_xxx.h
	    bOutput = (bOn)?(LED_GPIO_POWER_ACT_LEVEL):(!LED_GPIO_POWER_ACT_LEVEL);
		#elif defined(CFG_LED_BUTTON_HIGH_AS_ON)
	    bOutput = (bOn)?(1):(0);
	    #else
	    bOutput = (bOn)?(0):(1);
	    #endif

        AHC_ConfigGPIOPad(LED_GPIO_POWER, PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(LED_GPIO_POWER, AHC_TRUE);
		AHC_SetGPIO(LED_GPIO_POWER, bOutput);
	}
#endif
}

void LedCtrl_LaserLed(AHC_BOOL bOn)
{
#if defined(LED_GPIO_LASER) && (LED_GPIO_LASER != MMP_GPIO_MAX)
    AHC_BOOL bOutput;

    #ifdef LED_GPIO_LASER_ACT_LEVEL //may be defined in config_xxx.h
    bOutput = (bOn)?(LED_GPIO_LASER_ACT_LEVEL):(!LED_GPIO_LASER_ACT_LEVEL);
    #else
    bOutput = (bOn)?(0):(1);
    #endif
   if(pf_GetLaserLedStatus())
   	{
        AHC_ConfigGPIOPad(LED_GPIO_LASER, PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(LED_GPIO_LASER, AHC_TRUE);
		AHC_SetGPIO(LED_GPIO_LASER, bOutput);
   	}
#endif
}
#if defined(WIFI_PORT) && (WIFI_PORT == 1)
void LedCtrl_WiFiLed(AHC_BOOL bOn)
{  
#if defined(LED_GPIO_WIFI_STATE)
	if (LED_GPIO_WIFI_STATE != MMP_GPIO_MAX) {
        AHC_ConfigGPIOPad(LED_GPIO_WIFI_STATE, PAD_OUT_DRIVING(0));
		AHC_ConfigGPIO(LED_GPIO_WIFI_STATE, AHC_TRUE);
		AHC_SetGPIO(LED_GPIO_WIFI_STATE, (bOn)?(LED_GPIO_WIFI_ACT_LEVEL):(!LED_GPIO_WIFI_ACT_LEVEL));
	}
#endif
}
#endif

/*
 * The function helps to control LCD backlight ON/OFF when System up to escape
 * LCD's nonsense blinking 
 */
static AHC_BOOL	m_ubLcdBk_lock = 0;
static int		_ui_mmpPIP     = 1;
void LedCtrl_LcdBackLightLock(AHC_BOOL bLock)
{
	m_ubLcdBk_lock = bLock;
}

void LedCtrl_LcdBackLight(AHC_BOOL bOn)
{
	extern AHC_BOOL bInLCDPowerSave;
	
	if (m_ubLcdBk_lock)
		return;

	if (!TVFunc_IsInTVMode()	 &&
		!HDMIFunc_IsInHdmiMode() &&
		AHC_GetLCDBacklightGPIOpin() != CONFIG_PIO_REG_UNKNOWN)
    {
	    if(bOn)
	    {

	    	bBackLightStatus = 1;
	    
	    	if(bInLCDPowerSave)
	    		bInLCDPowerSave = AHC_FALSE;
	    }
	    else
	    {
	    	
	        bBackLightStatus = 0;
		}

		RTNA_LCD_Backlight(bBackLightStatus);        

	} else {
#if 0//def TV_ONLY		// TY Miao - TBD
	#if defined(CAR_DV)
		// NOP
	#else
	    AITPS_DSPY		pDSPY = AITC_BASE_DSPY;
	    static  int		_init = 0;
		static	int		ma, ov, pi;
		
		_ui_mmpPIP = bOn;
		if (bOn) {
			if (_init) {
				pDSPY->DSPY_MAIN_CTL = ma;
				pDSPY->DSPY_OVLY_CTL = ov;
				pDSPY->DSPY_PIP_CTL  = pi;
			}
		} else {
			_init = 1;
			ma = pDSPY->DSPY_MAIN_CTL;
			ov = pDSPY->DSPY_OVLY_CTL;
			pi = pDSPY->DSPY_PIP_CTL;
			pDSPY->DSPY_MAIN_CTL &= ~1;
			pDSPY->DSPY_OVLY_CTL &= ~1;
			pDSPY->DSPY_PIP_CTL  &= ~1;
		}
	#endif
#endif
	}
}

UINT8 LedCtrl_GetBacklightStatus(void)
{
	if(AHC_GetLCDBacklightGPIOpin() != MMP_GPIO_MAX) {
		return bBackLightStatus;
	} else {
		return _ui_mmpPIP;
	}
}

void LedCtrl_ButtonLed(int LED_Gpio, AHC_BOOL bOn)
{
    AHC_BOOL bOutput;
    
    if(LED_Gpio == MMP_GPIO_MAX)
    	return;

    #ifdef CFG_LED_BUTTON_HIGH_AS_ON //may be defined in config_xxx.h
    bOutput = (bOn)?(1):(0);
    #else
    bOutput = (bOn)?(0):(1);
    #endif
        
    AHC_ConfigGPIOPad(LED_Gpio, PAD_OUT_DRIVING(0));
	AHC_ConfigGPIO(LED_Gpio, AHC_TRUE);
	AHC_SetGPIO(LED_Gpio, bOutput);
}

void LedCtrl_FlickerLed(int LED_Gpio, UINT32 ultime)
{  
	int	t = 0;

    if (LED_Gpio != MMP_GPIO_MAX) {	
        AHC_ConfigGPIOPad(LED_Gpio, PAD_OUT_DRIVING(0));
    	AHC_ConfigGPIO(LED_Gpio, AHC_TRUE);

    	while(t < ultime)
    	{
    		AHC_SetGPIO(LED_Gpio, 1);
    		AHC_OS_SleepMs(200);
    		AHC_SetGPIO(LED_Gpio, 0);
    		AHC_OS_SleepMs(200);
    		t++;
    	}
    }
}
void LedCtrl_FlickerLedEx(int LED_Gpio, UINT32 ultime,UINT32 ums)
{
	int	t = 0;

    if (LED_Gpio != MMP_GPIO_MAX) {	
        AHC_ConfigGPIOPad(LED_Gpio, PAD_OUT_DRIVING(0));
    	AHC_ConfigGPIO(LED_Gpio, AHC_TRUE);

    	while(t < ultime)
    	{
    		AHC_SetGPIO(LED_Gpio, 1);
    		AHC_OS_SleepMs(ums);
    		AHC_SetGPIO(LED_Gpio, 0);
    		AHC_OS_SleepMs(ums);
    		t++;
    	}
    }
}

void LedCtrl_FlickerLedBeep(int LED_Gpio, UINT32 ultime, UINT32 ulMs)
{  
	int	t = 0;
	
    if (LED_Gpio != MMP_GPIO_MAX) {	
        AHC_ConfigGPIOPad(LED_Gpio, PAD_OUT_DRIVING(0));
    	AHC_ConfigGPIO(LED_Gpio, AHC_TRUE);

    	while(t < ultime)
    	{
    		AHC_BuzzerAlert(1000, 1, ulMs);
    		AHC_SetGPIO(LED_Gpio, 1);
    		AHC_OS_SleepMs(50);
    		AHC_SetGPIO(LED_Gpio, 0);
    		AHC_OS_SleepMs(50);
    		t++;
    	}
    }
}


void LedCtrl_FlickerLedByCustomer(AHC_PIO_REG LED_Gpio, UINT32 period, UINT32 times)
{
	if (MMP_GPIO_MAX == LED_Gpio)
		return;

	while (times--) {
		LedCtrl_ButtonLed(LED_Gpio, AHC_TRUE);
		AHC_OS_SleepMs(period >> 1);
		LedCtrl_ButtonLed(LED_Gpio, AHC_FALSE);
		AHC_OS_SleepMs(period >> 1);
	}
}

// Call by mmpf to enable/disable PIP window for BLANK DISPLAY OUPUT
int ui_mmpBacklightOff()
{
	return _ui_mmpPIP;
}

// Dummy function for compiler issue
void LedCtrl_ForceCloseTimer(AHC_BOOL enable)
{

}

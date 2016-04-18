/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "customer_config.h" 
#include "AHC_Common.h"
#include "AHC_Capture.h"
#include "AHC_General.h"
#include "AHC_Message.h"
#include "AHC_Sensor.h"
#include "AHC_Utility.h"
#include "AHC_GUI.h"
#include "AHC_Os.h"
#include "AHC_Menu.h"
#include "AHC_Version.h"
#include "AHC_Fs.h"
#include "AHC_Media.h"
#include "AHC_Parameter.h"
#include "AHC_Warningmsg.h"
#include "AHC_Display.h"
#include "AHC_General_CarDV.h"
#include "AHC_Video.h"
#include "AHC_USB.h"
#include "LedControl.h"
#include "ZoomControl.h"
#include "StateCameraFunc.h"
#include "dsc_charger.h"
#if (SUPPORT_GSENSOR) 
#include "GSensor_ctrl.h"
#endif
#if (SUPPORT_IR_CONVERTER)
#include "IR_Ctrl.h"
#endif
#if (SUPPORT_GPS) 
#include "GPS_ctl.h"
#endif
#if (TOUCH_UART_FUNC_EN)
#include "Touch_Uart_ctrl.h"
#endif
#include "disp_drv.h"
#include "AIT_Init.h"
#include "AHC_task.h"

#include "StateTVFunc.h"
#include "StateHDMIFunc.h"
#include "hdr_cfg.h"
#include "mmpf_system.h" //For task monitor //CarDV...
#include "PMUCtrl.h" // For PMUCtrl_Power_Off

/*===========================================================================
 * Global variable
 *===========================================================================*/ 

AHC_OS_SEMID 	ahc_System_Ready;
AHC_BOOL		gAHC_InitialDone = AHC_FALSE;    

#if (TASK_MONITOR_ENABLE)
MMPF_TASK_MONITOR gsTaskMonitorAhcMain;
MMPF_TASK_MONITOR gsTaskMonitorAHCWorkStack;
MMPF_TASK_MONITOR gsTaskMonitorAHCWorkJob;
#endif

/*===========================================================================
 * Extern variable
 *===========================================================================*/ 

extern MMP_BOOL 	CalibrationMode;
extern UARTCOMMAND 	sAhcUartCommand[];

/*===========================================================================
 * Extern function
 *===========================================================================*/

extern void 	AIHC_SetTempDCFMemStartAddr(UINT32 addr);
extern void 	AIHC_SetTempBrowserMemStartAddr(UINT32 addr);
extern void		InitKeyPad(void);
extern int      pf_HDR_EnSet(AHC_BOOL value);
extern void 	AHC_PrintAllTask(void);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

void InitOSDCustom(U32 colorFormat)
{
    UINT32 i;
    UINT32 iVal[10];
    INT32  bAlphaBlendingEn = 0;

#if (TVOUT_PREVIEW_EN)	
    if(AHC_IsTVConnectEx()){
        //TVFunc_InitPreviewOSD();
        return;
    }
#endif
#if (HDMI_PREVIEW_EN)
    if(AHC_IsHdmiConnect())
        return;		
#endif

    // LAYER 0 - ID 0 ~ 15
    // LAYER 1 - ID 16 ~ 31
    // Default layer is display id 0 & 16    

    for(i=1; i<2; i++){
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
        if (OSD_DISPLAY_SCALE_LCD) {
            AHC_OSDCreateBuffer(i,
            RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD, RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
            OSD_COLOR_RGB565_CCW/*OSD_COLOR_ARGB32_CCW*/);
        }
        else {
            AHC_OSDCreateBuffer(i, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, colorFormat);
        }
        
        iVal[0] = OSD_ROTATE_DRAW_NONE;        
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_ROTATE_BY_GUI, iVal);
        iVal[0] = OSD_FLIP_DRAW_NONE_ENABLE;       
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_FLIP_BY_GUI, iVal);        
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        //		printc("SET MAIN WINDOW %d %dx%d\r\n", i, PANEL_RES_H, PANEL_RES_W);
        if (OSD_DISPLAY_SCALE_LCD) {
            AHC_OSDCreateBuffer(i,
            RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD, RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
            OSD_COLOR_RGB565_CCW/*OSD_COLOR_ARGB32_CCW*/);
        }
        else {
            AHC_OSDCreateBuffer(i,
            RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH,
            OSD_COLOR_RGB565_CCW/*OSD_COLOR_ARGB32_CCW*/);
        }
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90)
        iVal[0] = OSD_ROTATE_DRAW_RIGHT_90;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        iVal[0] = OSD_ROTATE_DRAW_RIGHT_270;
#endif   
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_ROTATE_BY_GUI, iVal);
        iVal[0] = OSD_FLIP_DRAW_NONE_ENABLE;       
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_FLIP_BY_GUI, iVal);          		
#endif  		
        if (OSD_DISPLAY_SCALE_LCD) {
            iVal[0] = OSD_DISPLAY_SCALE_LCD - 1;    // remapping to enum MMP_DISPLAY_DUPLICATE
            iVal[1] = OSD_DISPLAY_SCALE_LCD - 1;
        }
        else {
            iVal[0] = MMP_DISPLAY_DUPLICATE_1X;
            iVal[1] = MMP_DISPLAY_DUPLICATE_1X;
        }

        AHC_OSDSetDisplayAttr(i, AHC_OSD_ATTR_DISPLAY_SCALE, iVal);
        AHC_OSDClearBuffer(i);
    }

    for(i=16; i<=20; i++)
    {
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
        if (OSD_DISPLAY_SCALE_LCD) {
            AHC_OSDCreateBuffer(i,
            RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD, RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
            colorFormat);
        }
        else {
            AHC_OSDCreateBuffer(i, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, colorFormat);
        }

        iVal[0] = OSD_ROTATE_DRAW_NONE;                
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_ROTATE_BY_GUI, iVal);
        iVal[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_FLIP_BY_GUI, iVal);          
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        //      printc("SET OVER WINDOW %d %dx%d\r\n", i + 16, PANEL_RES_H, PANEL_RES_W);
        if (OSD_DISPLAY_SCALE_LCD) {
            if (colorFormat == OSD_COLOR_RGB565) {
                AHC_OSDCreateBuffer(i,
                RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD, RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
                OSD_COLOR_RGB565_CCW);
            } else {
                AHC_OSDCreateBuffer(i,
                RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD, RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
                OSD_COLOR_ARGB32_CCW);
            }
        }
        else {
            if (colorFormat == OSD_COLOR_RGB565) {
                AHC_OSDCreateBuffer(i, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, OSD_COLOR_RGB565_CCW);
            } else {
                AHC_OSDCreateBuffer(i, RTNA_LCD_GetAttr()->usPanelW, RTNA_LCD_GetAttr()->usPanelH, OSD_COLOR_ARGB32_CCW);
            }
        }

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90)
        iVal[0] = OSD_ROTATE_DRAW_RIGHT_90;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        iVal[0] = OSD_ROTATE_DRAW_RIGHT_270;
#endif  
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_ROTATE_BY_GUI, iVal);
        iVal[0] = OSD_FLIP_DRAW_NONE_ENABLE;                
        AHC_OSDSetDisplayAttr(i,  AHC_OSD_ATTR_FLIP_BY_GUI, iVal);          
#endif
        if (OSD_DISPLAY_SCALE_LCD) {
            iVal[0] = OSD_DISPLAY_SCALE_LCD - 1;    // remapping to enum MMP_DISPLAY_DUPLICATE
            iVal[1] = OSD_DISPLAY_SCALE_LCD - 1;
        }
        else {
            iVal[0] = MMP_DISPLAY_DUPLICATE_1X;
            iVal[1] = MMP_DISPLAY_DUPLICATE_1X;
        }    
        AHC_OSDSetDisplayAttr(i, AHC_OSD_ATTR_DISPLAY_SCALE, iVal);
        AHC_OSDClearBuffer(i);
    }

    // Set transparent color
    iVal[0] = 1;         	// Enable
    iVal[1] = 0x00000000; 	// Black color
    AHC_OSDSetDisplayAttr(0, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    AHC_OSDSetDisplayAttr(1, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);

    for(i=0;i<5;i++){
        AHC_OSDSetDisplayAttr(i+16, AHC_OSD_ATTR_TRANSPARENT_ENABLE, iVal);
    }

    if(bAlphaBlendingEn){
        iVal[0] = 1;        // Enable
        iVal[1] = AHC_OSD_ALPHA_ARGB;

        for(i=0; i<8; i++) {
            iVal[2+i] = i * 256 / 8;  // Alpha weighting 
        }

        AHC_OSDSetDisplayAttr(0, AHC_OSD_ATTR_ALPHA_BLENDING_ENABLE, iVal);
        AHC_OSDSetDisplayAttr(1, AHC_OSD_ATTR_ALPHA_BLENDING_ENABLE, iVal);
        AHC_OSDSetDisplayAttr(2, AHC_OSD_ATTR_ALPHA_BLENDING_ENABLE, iVal);

        for(i=0;i<5;i++){
            AHC_OSDSetDisplayAttr(i+16, AHC_OSD_ATTR_ALPHA_BLENDING_ENABLE, iVal);
        }
    }    

#ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(1,  0x80000000); // Alpha value is 0x80
    AHC_OSDSetColor(0,  0x80000000); // Alpha value is 0x80
    AHC_OSDSetColor(20, 0x80000000); // Alpha value is 0x80
#else
    AHC_OSDSetColor(0x80000000); // Alpha value is 0x80
#endif

    AHC_OSDSetCurrentDisplay(1);    
    AHC_OSDSetCurrentDisplay(0);
    AHC_OSDSetCurrentDisplay(20);

    // Set MAIN Inactive & OVERLAY WINDOW Active
    AHC_OSDSetActive(0, 0);
    AHC_OSDSetActive(20, 1);

}

void InitOSD(void)
{
    U32 colorFormat;

	printc("### %s -\r\n", __func__);
	
    GUI_RegisterExternalFillRectCB((void *)AHC_OSD_ExternalFillRectCB);
    AHC_OSD_RegisterInitOSDCustomCB((void *)InitOSDCustom);

#if (TVOUT_PREVIEW_EN)
    if(AHC_IsTVConnectEx())
    {
#if 0//TBD
        if(MenuSettingConfig()->uiTVSystem==TV_SYSTEM_PAL)
        AHC_SetDisplayOutputDev(DISP_OUT_TV_PAL, AHC_DISPLAY_DUPLICATE_1X);
        else
        AHC_SetDisplayOutputDev(DISP_OUT_TV_NTSC, AHC_DISPLAY_DUPLICATE_1X);
#endif

        colorFormat = WMSG_LAYER_WIN_COLOR_FMT;
    }
    else
#endif	
#if (HDMI_PREVIEW_EN)
    if(AHC_IsHdmiConnect())
    {
        //AHC_SetDisplayOutputDev(DISP_OUT_HDMI, AHC_DISPLAY_DUPLICATE_1X);
        colorFormat = WMSG_LAYER_WIN_COLOR_FMT;
    }
    else
#endif
    {
        //Force OSD format as RGB565 for saving memory and drawing time.
        colorFormat = OSD_COLOR_RGB565;
    }

    AIHC_SetGUIMemStartAddr(AHC_GUI_TEMP_BASE_ADDR);
    
    if (OSD_DISPLAY_SCALE_LCD) {
        AHC_OSDInit(RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD,	RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
    				RTNA_LCD_GetAttr()->usPanelW / OSD_DISPLAY_SCALE_LCD,	RTNA_LCD_GetAttr()->usPanelH / OSD_DISPLAY_SCALE_LCD,
    				PRIMARY_DATESTAMP_WIDTH,		PRIMARY_DATESTAMP_HEIGHT, 
    				THUMB_DATESTAMP_WIDTH,			THUMB_DATESTAMP_HEIGHT, 
    				colorFormat,
    				AHC_TRUE);
    }
    else {
        AHC_OSDInit(RTNA_LCD_GetAttr()->usPanelW,	RTNA_LCD_GetAttr()->usPanelH,
    				RTNA_LCD_GetAttr()->usPanelW,	RTNA_LCD_GetAttr()->usPanelH,
    				PRIMARY_DATESTAMP_WIDTH,		PRIMARY_DATESTAMP_HEIGHT, 
    				THUMB_DATESTAMP_WIDTH,			THUMB_DATESTAMP_HEIGHT, 
    				colorFormat,
    				AHC_TRUE);
    }

}

UINT8 AHC_WaitForSystemReady(void)
{
    UINT16 SemCount = 0;

    AHC_OS_QuerySem(ahc_System_Ready, &SemCount); 

	return (0 == SemCount)?(0):(1);
}

void AHC_DCF_Init(void)
{
    AHC_AUDIO_I2S_CFG   ConfigAudioI2S;
    AHC_BOOL 			bSD_inserted = AHC_FALSE;

	printc("### %s -\r\n", __func__);
	
	AIHC_SetTempDCFMemStartAddr(AHC_DCF_TEMP_BASE_ADDR);
	AIHC_SetTempBrowserMemStartAddr(AHC_THUMB_TEMP_BASE_ADDR);

    if(MEDIA_SETTING_SD_CARD == MenuSettingConfig()->uiMediaSelect)
    {
        bSD_inserted = AHC_IsSDInserted();

        if(bSD_inserted){
			Enable_SD_Power(1 /* Power On */);
            AHC_SDMMC_SetState(SDMMC_IN);
            
            #if (FS_FORMAT_FREE_ENABLE)
            if( AHC_CheckMedia_FormatFree( AHC_MEDIA_MMC ) == AHC_FALSE )
            {
                SystemSettingConfig()->byNeedToFormatMediaAsFormatFree = 1;
            }
            else            
            #endif
                AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
        }
        else{
			Enable_SD_Power(0 /* Power Off */);
            AHC_SDMMC_SetState(SDMMC_OUT);
    	}
    }
    else
    {
        AHC_RemountDevices(MenuSettingConfig()->uiMediaSelect);
	}

    ConfigAudioI2S.workingMode      = AHC_AUDIO_I2S_MASTER_MODE;
    ConfigAudioI2S.mclkMode         = AHC_AUDIO_I2S_256FS_MODE;
    ConfigAudioI2S.outputBitSize    = AHC_AUDIO_I2S_OUT_16BIT;
    ConfigAudioI2S.lrckPolarity     = AHC_AUDIO_I2S_LRCK_L_LOW;

    AHC_ConfigAudioExtDevice(&ConfigAudioI2S, 44100);
	//MIMF_CUSTOMIZED_SORT_TYPE use for customized sorting type, implement by AHC_MIMF.c
#if(MIMF_CUSTOMIZED_SORT_TYPE)    
    AHC_MIMF_RegisterCusCallback();
#endif  
}

#if (TASK_MONITOR_ENABLE)
void AHC_Main_Task_ReSetMonitorTime(UINT32 ulTime)
{
    gsTaskMonitorAhcMain.ulExecTime = ulTime;
}
#endif

void AHC_Main_Task(void *p_arg)
{
    AHC_SWPACK_TaskInit();
    
    ahc_System_Ready = AHC_OS_CreateSem(0);

    AHC_USBDetectHandler();
#if (SUPPORT_IR_CONVERTER)
    #if 1
    printc("--I-- IR function is TBD\r\n");
    #else
    IRConverter_Initialize();
    // To Clear IR status of PowerOn Key avoid power off
    IRConverter_WriteCommand(IR_CONVERTER_CLEAR_POWERON, 0x01);
    #endif
#endif

	AHC_InitLCD(AHC_TRUE);

#if (TVOUT_PREVIEW_EN)
	if(AHC_IsTVConnectEx()) 
	{
		AHC_SetTVBooting(AHC_TRUE);
		AHC_InitTV(AHC_TRUE);
	}
#endif	
	
    AHC_Initialize();

	AHC_WaitForBootComplete();

    //AHC_USBDetectHandler();
   
#ifdef USB_LABLE_STRING
	AHC_SetUSBLable(USB_LABLE_STRING);
#endif

	/* Update Menu Setting */ 
	uiCheckDefaultMenuExist();

	MenuSettingConfig()->uiMediaSelect = MEDIA_SETTING_SD_CARD;

    #if (AHC_DRAM_SIZE == COMMON_DRAM_SIZE_32MB)

    pf_FCWS_EnSet(FCWS_EN_OFF);
    pf_SAG_EnSet(SAG_EN_OFF);
    pf_HDR_EnSet(HDR_EN_OFF);
    MMP_EnalbeHDR(MMP_FALSE);

    #endif

    #if SNR_CLK_POWER_SAVING_SETTING
    pf_HDR_EnSet(COMMON_HDR_EN_OFF);
    #endif

	Menu_SetVolume(MenuSettingConfig()->uiVolume);

	AHC_SetMenuFlashLEDStatus(MenuSettingConfig()->uiLEDFlash);	

#if (SUPPORT_GPS)
	AHC_OS_SetFlags(UartCtrlFlag, GPS_FLAG_INITIAL, AHC_OS_FLAG_SET);
#endif
#if (TOUCH_UART_FUNC_EN)
	AHC_OS_SetFlags(UartCtrlFlag, Touch_Uart_FLAG_INITIAL, AHC_OS_FLAG_SET);
#endif
	/*let power on logo off*/
	//AHC_SetDisplayMode(DISPLAY_MODE_DISABLE); //move to other loc
    // Remove this and initial this with jobdispatch task.//CZ patch...20160204
    //AHC_DCF_Init();

    #ifdef CFG_TURN_OFF_POWER_LED_WHEN_BOOT_FINISH
    LedCtrl_PowerLed(AHC_FALSE);
    #else
    LedCtrl_PowerLed(AHC_TRUE);
    #endif

    AHC_PowerOn_Welcome();

	#ifndef	NO_PANEL
	// InitOSD will make LCD off, Shutdown LCD backlight must before,
	// Otherwise, Some kind of LCD will show ugly white lines
	//LedCtrl_LcdBackLight(AHC_FALSE);
	//LedCtrl_LcdBackLightLock(AHC_TRUE);
	#endif	// NO_PANEL

    //InitOSD(); //Move to CE_JOB_DISPATCH_Task for fast boot.

    AHC_OS_ReleaseSem(ahc_System_Ready);

    //InitKeyPad(); //Move to CE_JOB_DISPATCH_Task for fast boot.
	
    AHC_UartRegisterUartCmdList(sAhcUartCommand);

#ifdef CFG_BOOT_FLASH_MODE //may be defined in config_xxx.h, could be FLASH_OFF or FLASH_ON
	// Always set Flash ON or OFF when system start up.
	#if CFG_BOOT_FLASH_MODE
	    MenuSettingConfig()->uiFlashMode = FLASH_ON;
	#else
	    MenuSettingConfig()->uiFlashMode = FLASH_OFF;
	#endif
#endif
#ifdef CFG_BOOT_MOV_CLIP_TIME //may be defined in config_xxx.h, could be MOVIE_CLIP_TIME_OFF or MOVIE_CLIP_TIME_xMIN
	MenuSettingConfig()->uiMOVClipTime 	 = CFG_BOOT_MOV_CLIP_TIME;
#endif
#ifdef CFG_BOOT_SLIDESHOW_FILE //may be defined in config_xxx.h, could be SLIDESHOW_FILE_STILL
	// Force slideshow photo only
	MenuSettingConfig()->uiSlideShowFile = SLIDESHOW_FILE_STILL; 
#endif
#ifdef CFG_BOOT_CHECK_USB_KEY_FILE //may be defined in config_xxx.h, a file name such as "SD:\\aituvc.txt"
	// Check USB Key file to run PCCAM
	AHC_CheckUSB_KeyFile(CFG_BOOT_CHECK_USB_KEY_FILE, 1 /* 1 for PCCAM */);
#endif
#ifdef CFG_BOOT_LCD_ROTATE //may be defined in config_xxx.h, could be LCD_ROTATE_OFF
    // Init LCD direction, always is normal
	MenuSettingConfig()->uiLCDRotate = CFG_BOOT_LCD_ROTATE;
#endif
#ifdef CFG_BOOT_FORCE_TURN_OFF_WIFI //	defined in config_xxx.h, for project of key control Wi-Fi On/Off
	Setpf_WiFi(WIFI_MODE_OFF);
#endif
#ifdef CFG_BOOT_FORCE_TURN_ON_WIFI	//	defined in config_xxx.h, for project of key control Wi-Fi On/Off
	Setpf_WiFi(WIFI_MODE_ON);
#endif
#ifdef CFG_VIDEO_FORMAT_SELECT_BY_SWITCH
	CFG_VIDEO_FORMAT_SELECT_BY_SWITCH();
#endif

	//gAHC_InitialDone = AHC_TRUE;
	/*//CZ patch...20160204
	{//for FS short name
        UINT64 FreeBytes;
        UINT32 TickCount;
        UINT32 seed;
        AHC_FS_GetStorageFreeSpace(AHC_GetMediaPath(), &FreeBytes);
        AHC_OS_GetTime(&TickCount);
        seed = (UINT16)(((FreeBytes>>32)+(FreeBytes%0xFFFFFFFF)+TickCount)%0xFFFFFFFF);
        srand(seed); 
    }
    */
	CalibrationMode  = 0;
    
    uiStateSystemStart();

	AHC_PreSetLCDDirection();

    #if 0//#ifdef CFG_BOOT_CHECK_USB_UPDATE_FILE //may be defined in config_xxx.h, a file name such as "SD:\\FLYTECAM.bin"
    if(AHC_IsUsbConnect()) {
	    if(IsAdapter_Connect()) {
		   SDUpdateCheckFileExisted(CFG_BOOT_CHECK_USB_UPDATE_FILE);
	    } 
	    else {
		    NotifyUSB_HwStatus(1);
	    }
    }	
    #endif

#if (VR_VIDEO_TYPE==COMMON_VR_VIDEO_TYPE_3GP)
    AHC_ConfigMovie(AHC_CLIP_CONTAINER_TYPE     , AHC_MOVIE_CONTAINER_3GP);
#elif (VR_VIDEO_TYPE==COMMON_VR_VIDEO_TYPE_AVI)
    AHC_ConfigMovie(AHC_CLIP_CONTAINER_TYPE     , AHC_MOVIE_CONTAINER_AVI);
#endif

#if (TASK_MONITOR_ENABLE)
    memcpy(&gsTaskMonitorAhcMain.TaskName[0], __func__, TASK_MONITOR_MAXTASKNAMELEN);
    gsTaskMonitorAhcMain.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_WAITING;
    gsTaskMonitorAhcMain.ulExecTime = 0;
    memset((void *)gsTaskMonitorAhcMain.ParaArray, 0x0, sizeof(gsTaskMonitorAhcMain.ParaArray));
    gsTaskMonitorAhcMain.ulParaLength = 0;    
    gsTaskMonitorAhcMain.pTimeoutCB = (TASK_MONITOR_TimeoutCallback *)NULL;
    MMPF_TaskMonitor_RegisterTask(&gsTaskMonitorAhcMain);
    //MMPF_TaskMonitor_RegisterGblCB((void *)&AHC_PrintAllTask);
    //MMPF_TaskMonitor_RegisterGblCB((void *) NULL);
#endif

#if defined(AIT_HW_WATCHDOG_ENABLE) && (AIT_HW_WATCHDOG_ENABLE)
        AHC_WD_Enable(AHC_TRUE);
#endif

#ifndef	NO_PANEL
		// InitOSD will make LCD off, Shutdown LCD backlight must before,
		// Otherwise, Some kind of LCD will show ugly white lines
		//LedCtrl_LcdBackLight(AHC_FALSE);
		//LedCtrl_LcdBackLightLock(AHC_TRUE);
#endif	// NO_PANEL

    while(1)
    {
        UINT32 uiMsgId, uiParam1, uiParam2;
		UINT16 usCount;

        while (1) {
    		AHC_GetAHLHPMessageCount(&usCount);
    		//printc("HPMCount = %d\r\n", usCount);
    		if(usCount < AHC_HP_MSG_QUEUE_SIZE) {
    			if(AHC_GetAHLMessage_HP( &uiMsgId, &uiParam1, &uiParam2) == AHC_FALSE) {
    			    if (AHC_GetAHLMessage( &uiMsgId, &uiParam1, &uiParam2) == AHC_FALSE) {
        			    continue;
    			    }

			        break;
    			}

			    break;
    		}
    		else if (AHC_GetAHLMessage( &uiMsgId, &uiParam1, &uiParam2) == AHC_FALSE) {
		        continue;
    		}

		    break;
        }

#if (TASK_MONITOR_ENABLE)
        MMPF_OS_GetTime(&gsTaskMonitorAhcMain.ulExecTime);
        gsTaskMonitorAhcMain.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_RUNNING;    

        *(MMP_ULONG *)(gsTaskMonitorAhcMain.ParaArray) = (MMP_ULONG)uiMsgId;
        gsTaskMonitorAhcMain.ulParaLength = sizeof(MMP_ULONG);  

        *(MMP_ULONG *)(gsTaskMonitorAhcMain.ParaArray + gsTaskMonitorAhcMain.ulParaLength) = (MMP_ULONG)uiParam1;
        gsTaskMonitorAhcMain.ulParaLength += sizeof(MMP_ULONG);  
        
        *(MMP_ULONG *)(gsTaskMonitorAhcMain.ParaArray + gsTaskMonitorAhcMain.ulParaLength) = (MMP_ULONG)uiParam2;
        gsTaskMonitorAhcMain.ulParaLength += sizeof(MMP_ULONG);          
#endif

        AHC_OS_AcquireSem(ahc_System_Ready, 0);

		uiStateMachine(uiMsgId, uiParam1, uiParam2);

        AHC_OS_ReleaseSem(ahc_System_Ready);

#if (TASK_MONITOR_ENABLE)
        gsTaskMonitorAhcMain.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_WAITING;
#endif
    }
}

void AHC_WorkStack_Task(void *p_arg)
{
	#define WORK_STACK_WAIT_UINT       50   // ms
	#define RTC_UPDATE_PERIOD          1    // ms, RTC_UPDATE_PERIOD * WORK_STACK_WAIT_UINT
	#define CHARGING_CHECK_PERIOD      3    // ms, CHARGING_CHECK_PERIOD * WORK_STACK_WAIT_UINT

    UINT32 uiTaskCounter = 0;

	AHC_WaitForBootComplete();

#if (TASK_MONITOR_ENABLE)
    memcpy(&gsTaskMonitorAHCWorkStack.TaskName[0], __func__, TASK_MONITOR_MAXTASKNAMELEN);
    gsTaskMonitorAHCWorkStack.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_WAITING;
    gsTaskMonitorAHCWorkStack.ulExecTime = 0;
    memset((void *)gsTaskMonitorAHCWorkStack.ParaArray, 0x0, sizeof(gsTaskMonitorAHCWorkStack.ParaArray)); 
    gsTaskMonitorAHCWorkStack.ulParaLength = 0;    
    gsTaskMonitorAHCWorkStack.pTimeoutCB = (TASK_MONITOR_TimeoutCallback *)NULL;       
    MMPF_TaskMonitor_RegisterTask(&gsTaskMonitorAHCWorkStack);
#endif
    
    while(1)
    {
    	// TODO: This task only for draw Battery Icon!?

        uiTaskCounter++;
#if (TASK_MONITOR_ENABLE)
        MMPF_OS_GetTime(&gsTaskMonitorAHCWorkStack.ulExecTime);
        gsTaskMonitorAHCWorkStack.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_RUNNING;    
        *(MMP_ULONG *)(gsTaskMonitorAHCWorkStack.ParaArray) = (MMP_ULONG)uiTaskCounter;
        gsTaskMonitorAHCWorkStack.ulParaLength = sizeof(MMP_ULONG); 
#endif            
        
        ///////////////////////////////////////////////////////////////////////////////
        // RTC Update Counter
        ///////////////////////////////////////////////////////////////////////////////
        if((uiTaskCounter % RTC_UPDATE_PERIOD) == 0) {
            AHC_SetParam(PARAM_ID_USE_RTC, 1);
            AHC_RTC_UpdateTime();
        }

#if OSD_SHOW_BATTERY_STATUS
        if ((uiTaskCounter % CHARGING_CHECK_PERIOD) == 0 && gAHC_InitialDone) {
    		if ((AHC_TRUE == AHC_GetChargerStatus()) && !AHC_Device_IsBusy())
    		{
    			AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_CHARGE_ICON, 0);
    		}
        }
#endif	// OSD_SHOW_BATTERY_STATUS

#if (TASK_MONITOR_ENABLE)
        gsTaskMonitorAHCWorkStack.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_WAITING;        
#endif
        AHC_OS_SleepMs(WORK_STACK_WAIT_UINT);
    }
}

void AHC_WorkJob_Task(void *p_arg)
{
    static MMP_ULONG Count = 0;
#if (GPS_CONNECT_ENABLE)    
    static MMP_ULONG ulGPSInfoRefreshCnt;
#endif

#if (TASK_MONITOR_ENABLE)
    memcpy(&gsTaskMonitorAHCWorkStack.TaskName[0], __func__, TASK_MONITOR_MAXTASKNAMELEN);
    gsTaskMonitorAHCWorkStack.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_WAITING;
    gsTaskMonitorAHCWorkStack.ulExecTime = 0;
    memset((void *)gsTaskMonitorAHCWorkStack.ParaArray, 0x0, sizeof(gsTaskMonitorAHCWorkStack.ParaArray)); 
    gsTaskMonitorAHCWorkStack.ulParaLength = 0;    
    gsTaskMonitorAHCWorkStack.pTimeoutCB = (TASK_MONITOR_TimeoutCallback *)NULL;       
    MMPF_TaskMonitor_RegisterTask(&gsTaskMonitorAHCWorkStack);
#endif

    while(1)
    {    
#if (TASK_MONITOR_ENABLE)
        MMPF_OS_GetTime(&gsTaskMonitorAHCWorkStack.ulExecTime);
        gsTaskMonitorAHCWorkStack.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_RUNNING;    
#endif            
    
        if(AHC_GetWaitIconState()) 
        {
    	    AHC_DrawWaitIcon();  
        }
        
        if(AHC_WMSG_States()) 
        {
            Count++;
            if(AHC_WMSG_IsTimeOut(Count*100)) {
                Count = 0;
            }    
        }

#if (GPS_CONNECT_ENABLE) 
		if( uiGetCurrentState() >= UI_CAMERA_MENU_STATE    &&
			uiGetCurrentState() <= UI_CLOCK_SETTING_STATE)
		{
			ulGPSInfoRefreshCnt++;
			if(ulGPSInfoRefreshCnt == 10)
			{
				ulGPSInfoRefreshCnt = 0;
				AHC_SendAHLMessage(AHLM_UI_NOTIFICATION, BUTTON_UPDATE_MESSAGE, 0);
			}
		}
#endif	     

#if defined(AIT_HW_WATCHDOG_ENABLE) && (AIT_HW_WATCHDOG_ENABLE)
        AHC_WD_Kick();
#endif

#if (TASK_MONITOR_ENABLE)
        gsTaskMonitorAHCWorkStack.sTaskMonitorStates = MMPF_TASK_MONITOR_STATES_WAITING;        
#endif
        AHC_OS_SleepMs(100);
    }
}

#if 0
void ________Uart_Function_________(){ruturn;} //dummy
#endif

void UartCmdSetMode( char* szParam )
{
    int iMode;

    sscanfl( szParam, "%x",&iMode);

    if( iMode == 0xFF )
    {
        printc("AHC_MODE_IDLE = 0x00,              \n");
        printc("AHC_MODE_CAPTURE_PREVIEW = 0x10,   \n");
        printc("AHC_MODE_DRAFT_CAPTURE = 0x11,     \n");
        printc("AHC_MODE_STILL_CAPTURE = 0x20,     \n");
        printc("AHC_MODE_C2C_CAPTURE = 0x21,       \n");
        printc("AHC_MODE_SEQUENTIAL_CAPTURE = 0x22,\n");
        printc("AHC_MODE_USB_MASS_STORAGE = 0x30,  \n");
        printc("AHC_MODE_USB_WEBCAM = 0x31,        \n");
        printc("AHC_MODE_PLAYBACK  = 0x40,         \n");
        printc("AHC_MODE_THUMB_BROWSER = 0x50,     \n");
        printc("AHC_MODE_VIDEO_RECORD = 0x60,      \n");
        printc("AHC_MODE_RECORD_VONLY = 0x61,      \n");
        printc("AHC_MODE_RECORD_PREVIEW = 0x62,    \n");
        printc("AHC_MODE_CALIBRATION = 0xF0,       \n");
        printc("AHC_MODE_MAX = 0xFF                \n");
    }
    else
    {
        AHC_SetMode( iMode );
    }  
}

void UartCmdSetUiMode( char* szParam )
{
    int uiMode;

    sscanfl( szParam, "%x",&uiMode);

    if( uiMode == 0xFF )
    {
        printc("UI_CAMERA_STATE    		= 0x00, \n");
        printc("UI_VIDEO_STATE     		= 0x01, \n");
        printc("UI_BROWSER_STATE   		= 0x02, \n");
        printc("UI_PLAYBACK_STATE  		= 0x03, \n");
        printc("UI_SLIDESHOW_STATE 		= 0x04, \n");
        printc("UI_MSDC_STATE      		= 0x05, \n");
        printc("UI_HDMI_STATE      		= 0x06, \n");
        printc("UI_TVOUT_STATE     		= 0x07, \n");
        printc("UI_CAMERA_MENU_STATE   	= 0x08, \n");
        printc("UI_VIDEO_MENU_STATE    	= 0x09, \n");
        printc("UI_PLAYBACK_MENU_STATE 	= 0x0A, \n");
    }
    else
    {
        StateSwitchMode( uiMode );
    }   
}

void UartCmd_Main(char* szParam)
{
    UINT16 uiItem   = 0;
    UINT16 uiParam1 = 0;
    UINT16 uiParam2 = 0;
    UINT16 uiParam3 = 0;
    UINT16 uiParam4 = 0;
    UINT16 uiParam5 = 0;
    UINT16 uiParam6 = 0;
	
	sscanfl( szParam, "%d %d %d %d %d %d %d",&uiItem, &uiParam1, &uiParam2, &uiParam3, &uiParam4, &uiParam5, &uiParam6 );

    switch( uiItem ) 
    {
        case 0:     
            break;
            
        case 1: 
            break;
                
        case 2: 
            break;
    }
}

UARTCOMMAND sAhcUartCommand[] =
{
    { "mode",          " Mode ",   		   "AHC_SetMode",      			UartCmdSetMode }, 
    { "uimode",        " uiMode ",   	   "UI_SetMode",      			UartCmdSetUiMode },
    { "ucm",           "",                 "Test Uart command",         UartCmd_Main},
    {0,0,0,0}
};

AHC_BOOL MenuFunc_CheckAvailableSpace( UINT8 uiCameraState )
{
    AHC_BOOL bIsAvailableSpace = AHC_TRUE;

    printc("### %s -\r\n", __func__);
        
    switch( uiCameraState )
    {
    case UI_VIDEO_STATE:
        {
            UBYTE bHour = 0, bMin = 0, bSec = 0;

            AHC_AvailableRecordTime(&bHour, &bMin, &bSec);
            
            if ((bHour + bMin + bSec) == 0)
            {
                bIsAvailableSpace = AHC_FALSE;
            }
        }
        break;

    case UI_CAMERA_BURST_STATE:
        /*
        // TBD
        if( CameraSettingConfig()->uiRemainingShots < CaptureBurstFunc_GetShotNumber( MenuSettingConfig()->uiBurstShot ) )
        {
            bIsAvailableSpace = AHC_FALSE;
        }
        */
        break;

    case UI_TVOUT_STATE:
        if (TVFunc_Status() == AHC_TV_VIDEO_PREVIEW_STATUS)
        {
            UBYTE bHour = 0, bMin = 0, bSec = 0;

            AHC_AvailableRecordTime(&bHour, &bMin, &bSec);
            
            if ((bHour + bMin + bSec) == 0)
            {
                bIsAvailableSpace = AHC_FALSE;
            }
        }
        else
        {
            UINT64 freeBytes = 0;
            UINT32 remainCaptureNum = 0;
            
            CaptureFunc_CheckMemSizeAvailable(&freeBytes, &remainCaptureNum);

            if (remainCaptureNum)
            {
                bIsAvailableSpace = AHC_FALSE;
            }
        }
        break;
            

    case UI_HDMI_STATE:
        if (TVFunc_Status() == AHC_HDMI_VIDEO_PREVIEW_STATUS)
        {
            UBYTE bHour = 0, bMin = 0, bSec = 0;

            AHC_AvailableRecordTime(&bHour, &bMin, &bSec);
            
            if ((bHour + bMin + bSec) == 0)
            {
                bIsAvailableSpace = AHC_FALSE;
            }
        }
        else
        {
            UINT64 freeBytes = 0;
            UINT32 remainCaptureNum = 0;
            
            CaptureFunc_CheckMemSizeAvailable(&freeBytes, &remainCaptureNum);

            if (remainCaptureNum)
            {
                bIsAvailableSpace = AHC_FALSE;
            }
        }
        break;

    default:
        {
            UINT64 freeBytes = 0;
            UINT32 remainCaptureNum = 0;
            
            CaptureFunc_CheckMemSizeAvailable(&freeBytes, &remainCaptureNum);

            if (remainCaptureNum)
            {
                bIsAvailableSpace = AHC_FALSE;
            }
        }
        break;
    }

    return bIsAvailableSpace;
}


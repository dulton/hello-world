//==============================================================================
//
//  File        : AHC_USB.c
//  Description : AHC USB function
//  Author      :
//  Revision    : 1.0
//
//==============================================================================

/*===========================================================================
 * Include files
 *===========================================================================*/
#include "AHC_USB.h"
#include "mmps_pio.h"
#include "ait_transport.h"
#include "ahc_general.h"
#include "mmps_system.h"
#include "mmp_usbctl_inc.h"
#include "usb_cfg.h"
#include "Mmpf_usbvend.h" //For "USB_DETECT_MODE"
#include "mmpf_usbmsdc.h"
#include "AHC_USBHost.h"

/*===========================================================================
 * Global varible
 *===========================================================================*/
#ifndef USB_IN_DETECT_VBUS_PIN
#error "Please define USB_IN_DETECT_VBUS_PIN!"
#endif
#ifndef USB_OUT_PWR_SW_PIN
#error "Please define USB_OUT_PWR_SW_PIN!  If the project does not support rear camera, define it as MMP_GPIO_MAX."
#endif

static const struct USB_DETECT_ROUTINE _usb_det_routine = {
    USB_DETECT_PHASE_INIT,
    USB_DETECT_PHASE_INIT,
    200,
    USB_DETECTION_DISABLE_TIMEOUT_CNT,
    USB_DETECTION_DISABLE_TIMEOUT_CNT,
    (!USB_IN_DETECT_VBUS_PIN_ACTIVE) & 0x01,
    0,
    NULL,
    NULL,
    AHC_USBDetectRoutine
};

static const struct USB_DETECT_TYPE _usb_det_type[USB_DETECT_PHASE_MAX] = {
    //{&_usb_det_routine, sizeof(struct USB_DETECT_TYPE), USB_DETECT_PHASE_OP_NONE, AHC_USB_SizeOf, AHC_USB_SubRoutine, NULL, NULL, NULL},
    {&_usb_det_routine, USB_DETECT_PHASE_INIT,          AHC_USB_SubRoutine, AHC_USB_INIT_Detect,            AHC_USB_INIT_Exec,          AHC_USB_INIT_Notify},
#if USB_DETECTION_CUSTOM
    {&_usb_det_routine, USB_DETECT_PHASE_CHECK_VBUS,    AHC_USB_SubRoutine, AHC_USB_Check_VBUS_Custom_Detect,      AHC_USB_Check_VBUS_Custom_Exec,    AHC_USB_Check_VBUS_Custom_Notify},
    {&_usb_det_routine, USB_DETECT_PHASE_CHECK_USBID,   AHC_USB_SubRoutine, AHC_USB_Check_USBID_Custom_Detect,     AHC_USB_Check_USBID_Custom_Exec,   AHC_USB_Check_USBID_Custom_Notify},
#else
    {&_usb_det_routine, USB_DETECT_PHASE_CHECK_VBUS,    AHC_USB_SubRoutine, AHC_USB_Check_VBUS_Detect,      AHC_USB_Check_VBUS_Exec,    AHC_USB_Check_VBUS_Notify},
    {&_usb_det_routine, USB_DETECT_PHASE_CHECK_USBID,   AHC_USB_SubRoutine, AHC_USB_Check_USBID_Detect,     AHC_USB_Check_USBID_Exec,   AHC_USB_Check_USBID_Notify},
#endif
    {&_usb_det_routine, USB_DETECT_PHASE_OTG_SESSION,   AHC_USB_SubRoutine, AHC_USB_OTG_Session_Detect,     AHC_USB_OTG_Session_Exec,   AHC_USB_OTG_Session_Notify},
    {&_usb_det_routine, USB_DETECT_PHASE_VBUS_ACTIVE,   AHC_USB_SubRoutine, AHC_USB_VBUS_Active_Detect,     AHC_USB_VBUS_Active_Exec,   AHC_USB_VBUS_Active_Notify},
#if USB_DETECTION_CUSTOM
    {&_usb_det_routine, USB_DETECT_PHASE_CHECK_CONN,    AHC_USB_SubRoutine, AHC_USB_Check_Connect_Custom_Detect,   AHC_USB_Check_Connect_Exec, AHC_USB_Check_Connect_Notify},
    {&_usb_det_routine, USB_DETECT_PHASE_REAR_CAM,      AHC_USB_SubRoutine, AHC_USB_Check_Connect_Custom_Detect,   AHC_USB_Check_Connect_Exec, AHC_USB_Rearcam_Notify},
    {&_usb_det_routine, USB_DETECT_PHASE_REAR_CAM_DISCONN,      AHC_USB_SubRoutine, AHC_USB_RearCam_Disconn_Custom_Detect, AHC_USB_RearCam_Disconn_Custom_Exec, AHC_USB_RearCam_Disconn_Custom_Notify},    
#else
    {&_usb_det_routine, USB_DETECT_PHASE_CHECK_CONN,    AHC_USB_SubRoutine, AHC_USB_Check_Connect_Detect,   AHC_USB_Check_Connect_Exec, AHC_USB_Check_Connect_Notify},
    {&_usb_det_routine, USB_DETECT_PHASE_REAR_CAM,      AHC_USB_SubRoutine, AHC_USB_Check_Connect_Detect,   AHC_USB_Check_Connect_Exec, AHC_USB_Rearcam_Notify},
#endif
    {&_usb_det_routine, USB_DETECT_PHASE_NG,            AHC_USB_SubRoutine, NULL,                           NULL,                       NULL},
};

#define _AHC_USB_CHECK_POINTER_VALID(p) if(p==NULL){printc("%s,%d error!\r\n",__func__,__LINE__);while(1);}
#define _AHC_USB_ASSERT {printc("%s,%d error!\r\n",__func__,__LINE__);while(1);}
#define _AHC_USB_PRINTC(...) //printc(__VA_ARGS__) //Eanble it in debug mode.

AHC_BOOL gbAHC_USBDetectionHighPriority = AHC_TRUE;
/*===========================================================================
 * Extern function
 *===========================================================================*/

#if (MTP_FUNC == 1)
extern MMP_USHORT 	usb_initialize(USB_STATE *pState);
extern MMP_USHORT 	usb_checkcommand(USB_STATE *pState);
extern MMP_USHORT 	usb_receivedata(USB_STATE *pState, MMP_UBYTE *pBuffer, MMP_ULONG size, MMP_ULONG *pCount, USB_TRANSFER *state);
extern MMP_USHORT 	usb_senddata(USB_STATE *pState, MMP_UBYTE *pBuffer, MMP_ULONG size, MMP_ULONG *pCount, USB_TRANSFER state);
extern MMP_USHORT 	usb_sendevent(USB_STATE *pState, MMP_UBYTE *pBuffer, MMP_ULONG size, MMP_USHORT *count);
extern MMP_USHORT 	usb_disconnect(USB_STATE *pState);
extern MMP_USHORT 	usb_connect(USB_STATE *pState);
extern MMP_USHORT 	usb_reset(USB_STATE *pState);
extern void 		usb_stall(USB_STATE *pState);
extern void 		usb_unstall(USB_STATE *pState);
extern MMP_USHORT 	usb_isconnected(USB_STATE *pState);
#endif

extern void         USB_BDevice_In(UINT8 in);
extern MMP_ULONG	m_glOpenFileCount;
extern AHC_BOOL     gbAhcDbgBrk;
/*===========================================================================
 * Function prototype
 *===========================================================================*/

#if 0
void __INTERNAL_API__(){}
#endif

AHC_BOOL AHC_DisconnectDevice(void)
{
    MMP_ERR sRet = MMP_ERR_NONE;

    AHC_SetUsbMode(AHC_USB_MODE_NONE);
    sRet = MMPS_USB_StopDevice();
    sRet |= MMPS_USB_DisconnectDevice();

    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}    

    return AHC_TRUE;
}

#if 0
void __AHC_USB_API__(){}
#endif

/**
 @brief Set USB operating mode

 This function setup the USB operating mode and return execution status.
 Parameters:
 @param[in] byMode: the enum value of current support USB operating mode
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetUsbMode(AHC_USB_OP_MODE byMode)
{
    //AHC_BOOL 		    bRetStatus = AHC_FALSE;
    AHC_USB_OP_MODE     USBOpMode;
    MMPS_USB_OP_MODE    mmps_op_mode = MMPS_USB_NONE_MODE;
    MMP_ERR             sRet = MMP_ERR_NONE;
    UINT32 ulTimeout = 0xB0000000; //Based on 528MHz CPU, it is about 5 secs.  
    
    //if USB device is running, then disconnect device before change mode;
    if ((AHC_IsUsbConnect() == AHC_TRUE) && (AHC_GetUsbMode(&USBOpMode) == AHC_SUCCESS)) {
        if (USBOpMode == byMode) {
            RTNA_DBG_Str(0, "USBOpMode is same\r\n");
            return AHC_TRUE;
        }
        else if (USBOpMode != AHC_USB_MODE_NONE) {
            printc("%s,%d USB mode is not idle!\r\n",__func__,__LINE__);
            //AHC_UsbConnect(AHC_FALSE);
        }
    }

    switch(byMode){
        case AHC_USB_MODE_WEBCAM:
            mmps_op_mode = MMPS_USB_PCCAM_MODE;
            break;
        case AHC_USB_MODE_MASS_STORAGE:
            mmps_op_mode = MMPS_USB_MSDC_MODE;
            break;
        case AHC_USB_MODE_DPS_MODE:
            mmps_op_mode = MMPS_USB_DPS_MODE;
            break;
        case AHC_USB_MODE_PTP_MODE:
            mmps_op_mode = MMPS_USB_PTP_MODE;
            break;
        case AHC_USB_MODE_PCSYNC_MODE:
            mmps_op_mode = MMPS_USB_PCSYNC_MODE;
            break;
        case AHC_USB_MODE_MTP_MODE:
            mmps_op_mode = MMPS_USB_MTP_MODE;
            break;
        case AHC_USB_MODE_NONE:
        case AHC_USB_MODE_MAX:
        default:
            printc("%s,%d,%d error!\r\n", __func__, __LINE__,byMode);
            break;
    }

    if(mmps_op_mode == MMPS_USB_NONE_MODE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,byMode); return AHC_FALSE;}    

    sRet = MMPS_USB_AdjustVref(gususb_vref);
    sRet |= MMPS_USB_AdjustBiasCurrent(gususb_bias_current);
    sRet |= MMPS_USB_AdjustSignal(gususb_tx_cur, gususb_tx_cur_plus, gususb_sq_level);    
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}    

    if (mmps_op_mode == MMPS_USB_MSDC_MODE) {
        if (m_glOpenFileCount) {
            do {
                RTNA_DBG_Str(0, "MMPS_System_AllocateFSBuffer wait m_glOpenFileCount to be zero\r\n");
                if((m_glOpenFileCount == 0))    				
                break;    			
            }while(m_glOpenFileCount && (--ulTimeout > 0));
            
            if(ulTimeout == 0){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE; }
            sRet = MMPS_System_AllocateFSBuffer(MMPS_SYSTEM_APMODE_DSC);
        }
        else{
            sRet = MMPS_System_AllocateFSBuffer(MMPS_SYSTEM_APMODE_DSC);
        }
    }
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}    
    
    sRet = MMPS_USB_SetMode(mmps_op_mode);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}    
       
    return AHC_TRUE;
}

/**
 @brief Get and return current USB operating mode

 This function is used to get the current working USB operating mode.
 Parameters:
 @param[in] *puiMode: pointer which is used to pass the enum value of current USB operating mode.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetUsbMode(AHC_USB_OP_MODE *puiMode)
{
    MMP_ERR sRet = MMP_ERR_NONE;
    AHC_BOOL ahcRet = AHC_FALSE;
    MMPS_USB_OP_MODE usbMode;

    //default mode.
    *puiMode = AHC_USB_MODE_NONE;

    sRet = MMPS_USB_GetMode(&usbMode);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); /*return AHC_FALSE;*/}   
        
    switch(usbMode){
        case MMPS_USB_PCCAM_MODE:
            *puiMode = AHC_USB_MODE_WEBCAM;
            ahcRet = AHC_TRUE;
            break;
        case MMPS_USB_MSDC_MODE:
            *puiMode = AHC_USB_MODE_MASS_STORAGE;
            ahcRet = AHC_TRUE;
            break;
        case MMPS_USB_DPS_MODE:
            *puiMode = AHC_USB_MODE_DPS_MODE;
            ahcRet = AHC_TRUE;
            break;                
        case MMPS_USB_PTP_MODE:
            *puiMode = AHC_USB_MODE_PTP_MODE;
            ahcRet = AHC_TRUE;
            break;     
        case MMPS_USB_PCSYNC_MODE:
            *puiMode = AHC_USB_MODE_PCSYNC_MODE;
            ahcRet = AHC_TRUE;
            break;                
        case MMPS_USB_MTP_MODE:
            *puiMode = AHC_USB_MODE_MTP_MODE;
            ahcRet = AHC_TRUE;
            break;                
        case MMPS_USB_DETECT_MODE:
            *puiMode = AHC_USB_MODE_DETECT_MODE;
            ahcRet = AHC_TRUE;
            break;                
        case MMPS_USB_ISOTEST_MODE:
            *puiMode = AHC_USB_MODE_ISOTEST_MODE;
            ahcRet = AHC_TRUE;
            break;                            
        case MMPS_USB_ADAPTER_MODE:
            *puiMode = AHC_USB_MODE_ADAPTER_MODE;
            ahcRet = AHC_TRUE;
            break;                            
        case MMPS_USB_NONE_MODE:   
        case MMPS_USB_MAX_MODE:
        //case MMPS_USB_MSDC_AIT_DEBUG_MODE:
        default:
            ahcRet = AHC_FALSE;
            break;
    }
    
    return ahcRet;
}

AHC_BOOL AHC_USB_WaitUntilFirstRoundDone(UINT32 ulTimeoutMs)
{
    void *pusb_obj=NULL;
    //UINT16 cur_phase;
    USB_DETECT_PHASE USBCurPhase, USBNextPhase;

    ulTimeoutMs /= 10;
    do{
        pusb_obj = AHC_USBGetCurTypeObj(0);
        USBCurPhase = (USB_DETECT_PHASE)AHC_USBGetPhase(pusb_obj, AHC_USB_GET_PHASE_CURRENT);        
        USBNextPhase = (USB_DETECT_PHASE)AHC_USBGetPhase(pusb_obj, AHC_USB_GET_PHASE_NEXT);        
        AHC_OS_Sleep(10);   
    }while((((USBCurPhase == USB_DETECT_PHASE_CHECK_VBUS) && (USBNextPhase == USB_DETECT_PHASE_VBUS_ACTIVE)) ||
        (USBNextPhase == USB_DETECT_PHASE_CHECK_USBID) || 
        (USBNextPhase == USB_DETECT_PHASE_OTG_SESSION) ||
        (USBNextPhase == USB_DETECT_PHASE_CHECK_CONN)) && 
        (--ulTimeoutMs > 0));

    if(ulTimeoutMs > 0){
        return AHC_TRUE;
    }

    return AHC_FALSE;
}

/**
 @brief 

 This function
 Parameters:
 @param[in]: 
 @retval void
*/
void AHC_USB_PauseDetection(UINT8 bPauseDetection)
{
    void *pusb_obj=NULL;
    struct USB_DETECT_TYPE *pdet_type = NULL;
    struct USB_DETECT_ROUTINE *pusb_det_routine = NULL;

    pusb_obj = AHC_USBGetCurTypeObj(0);
    pdet_type = (struct USB_DETECT_TYPE *)pusb_obj;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    pusb_det_routine->pause_detection = bPauseDetection;
}

/**
 @brief Check the connection of USB device

 This function
 Parameters:
 @param[in]: ulM: The max period that host try to connect with B device.  Unit: milliseconds.
 @retval void
*/
void AHC_USB_SetDetectBDeviceTimeout(UINT32 ulMs)
{
    void *pusb_obj=NULL;
    struct USB_DETECT_TYPE *pdet_type = NULL;
    struct USB_DETECT_ROUTINE *pusb_det_routine = NULL;

    pusb_obj = AHC_USBGetCurTypeObj(0);
    pdet_type = (struct USB_DETECT_TYPE *)pusb_obj;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    /*
    USB_DETECT_PHASE_CHECK_VBUS -> USB_DETECT_PHASE_CHECK_USBID: USBH_CHECK_ID_TIMEOUT
    USB_DETECT_PHASE_CHECK_USBID -> USB_DETECT_PHASE_OTG_SESSION: WAIT_OTG_SESSION_INTERVAL
    USB_DETECT_PHASE_OTG_SESSION -> USB_DETECT_PHASE_CHECK_CONN: WAIT_CHECK_BDEVICE_CONN_INTERVAL
    USB_DETECT_PHASE_CHECK_CONN -> USB_DETECT_PHASE_CHECK_VBUS: POLLING_VBUS_INTERVAL
    */    
    
    pusb_det_routine->timeout_init_val = pusb_det_routine->timeout = ((ulMs+
        (USBH_CHECK_ID_TIMEOUT+WAIT_OTG_SESSION_INTERVAL+WAIT_CHECK_BDEVICE_CONN_INTERVAL+POLLING_VBUS_INTERVAL) -1 ) /
        (USBH_CHECK_ID_TIMEOUT+WAIT_OTG_SESSION_INTERVAL+WAIT_CHECK_BDEVICE_CONN_INTERVAL+POLLING_VBUS_INTERVAL));   
}

/**
 @brief Check the connection of USB device

 This function
 Parameters:
 @param[in]: void
 @retval AHC_USB_STATUS: AHC_USB_NORMAL, AHC_USB_ISADAPTER, AHC_USB_IDLE
*/
AHC_USB_STATUS AHC_USB_GetLastStatus(void)
{
	return MMPS_USB_GetLastStatus();
}

/**
 @brief Check the connection of USB device

 This function
 Parameters:
 @param[in]: void
 @retval AHC_TRUE:Connect AHC_FALSE: NO connection.
*/
AHC_BOOL AHC_IsUsbConnect(void)
{
    void *pusb_obj=NULL;
    UINT16 cur_phase;
    UINT8 bVBus_State;
    UINT16 ulTimeout=50;
    //MMP_UBYTE   ubVbusSR;
   
    pusb_obj = AHC_USBGetCurTypeObj(0);
    cur_phase = AHC_USBGetPhase(pusb_obj, AHC_USB_GET_PHASE_CURRENT);
    
    //Under initiating...
    while((USB_DETECT_PHASE_INIT == cur_phase) && (--ulTimeout > 0)){
        cur_phase = AHC_USBGetPhase(pusb_obj, AHC_USB_GET_PHASE_CURRENT);
        AHC_OS_Sleep(10);
    }

    if(ulTimeout == 0){
        printc("%s,%s timeout.\r\n", __func__, __LINE__);
    }

    bVBus_State = AHC_USBGetVBUS(pusb_obj);
    
    if(USB_IN_DETECT_VBUS_PIN_ACTIVE==bVBus_State){
        return AHC_TRUE;
    }
    
    return AHC_FALSE;
}

/**
 @brief Check the connection of DC Cable

 This function
 Parameters:
 @param[in]: void
 @retval AHC_TRUE:Connect AHC_FALSE: NO connection.
*/
AHC_BOOL AHC_IsDcCableConnect(void)
{
#if defined(DEVICE_GPIO_DC_INPUT)
    UINT8 bStatus;

    if (DEVICE_GPIO_DC_INPUT != MMP_GPIO_MAX) {
        AHC_ConfigGPIOPad(DEVICE_GPIO_DC_INPUT, PAD_NORMAL_TRIG);
        AHC_ConfigGPIO(DEVICE_GPIO_DC_INPUT, AHC_FALSE);
        AHC_GetGPIO(DEVICE_GPIO_DC_INPUT, &bStatus);

        return (DEVICE_GPIO_DC_INPUT_LEVEL == bStatus) ? (AHC_TRUE) : (AHC_FALSE);
	}
#endif

	return AHC_FALSE;
}

AHC_BOOL AHC_IsUsbBooting(void)
{
    return (((AHC_GetBootingSrc() & PWR_ON_BY_VBUS) == PWR_ON_BY_VBUS) ? AHC_TRUE : AHC_FALSE);
}

AHC_BOOL AHC_IsDCInBooting(void)
{
    return (((AHC_GetBootingSrc() & PWR_ON_BY_DC) == PWR_ON_BY_DC) ? AHC_TRUE : AHC_FALSE);
}

// CarDV
#if (MSDC_SUPPORT_SECRECY_LOCK)
AHC_BOOL AHC_SetMsdcSecrecyLock(AHC_BOOL bLockMode)
{
    MMPF_MSDC_SetSecrecyLock((MMP_BOOL) bLockMode);
    return AHC_TRUE;
}

AHC_BOOL AHC_GetMsdcSecrecyLock(void)
{
    return (AHC_BOOL) MMPF_MSDC_GetSecrecyLock();
}
#endif

#if (MSDC_SUPPORT_AIT_CUSTOMER_SCSI_CMD) 
AHC_BOOL AHC_AitScsiCustomerCmd_Register_CB(MSDC_AIT_SCSI_CUSTOMER_CB cb)
{
	MMPF_MSDC_AITSCSI_Customer_Register_CB(cb);
    return AHC_TRUE;
}
#endif

#if 0
void _USB_PTP_RELATE_FUNC_(){}
#endif

#if (MTP_FUNC == 1)
/**
 @brief Initialize PTP protocal

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpInitialize(USB_STATE* pState)//(AHC_PTP_STATE *pState)
{
	MMP_USHORT retStatus;

	retStatus = usb_initialize(pState);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief check PTP command

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpCheckCommand(USB_STATE *pState)
{
	MMP_USHORT retStatus;

	retStatus = usb_checkcommand(pState);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief Receive PTP data

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpReceiveData(USB_STATE *pState, UINT8 *pBuffer, UINT32 size, UINT32 *pCount, USB_TRANSFER *state)
{
	MMP_USHORT retStatus;

	retStatus = usb_receivedata(pState, (MMP_UBYTE*)pBuffer, (MMP_ULONG)size, (MMP_ULONG*)pCount, state);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief send PTP data

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpSendData(USB_STATE *pState, UINT8 *pBuffer, UINT32 size, UINT32 *pCount, USB_TRANSFER state)
{
	MMP_USHORT retStatus;

	retStatus = usb_senddata(pState, (MMP_UBYTE*)pBuffer, (MMP_ULONG)size, (MMP_ULONG*)pCount, state);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief send PTP event

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpSendEvent(USB_STATE *pState, UINT8 *pBuffer, UINT32 size, UINT16 *count)
{
	MMP_USHORT retStatus;

	retStatus = usb_sendevent(pState, (MMP_UBYTE*)pBuffer, (MMP_ULONG)size, (MMP_USHORT*)count);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief disconnect ptp

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpDisconnect(USB_STATE *pState)
{
	MMP_USHORT retStatus;

	retStatus = usb_disconnect(pState);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief connect PTP

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpConnect(USB_STATE *pState)
{
	MMP_USHORT retStatus;

	retStatus = usb_disconnect(pState);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief reset PTP

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpReset(USB_STATE *pState)
{
	MMP_USHORT retStatus;

	retStatus = usb_reset(pState);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

/**
 @brief stall PTP transfer

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpStall(USB_STATE *pState)
{
	usb_stall(pState);

	return AHC_TRUE;
}

/**
 @brief unstall PTP transfer

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpUnstall(USB_STATE *pState)
{
	usb_stall(pState);

	return AHC_TRUE;
}

/**
 @brief check if PTP is connected

 This function
 Parameters:
 @param[in]
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PtpIsConnected(USB_STATE *pState)
{
	MMP_USHORT retStatus;

	retStatus = usb_isconnected(pState);

	if (retStatus)
		return AHC_FALSE;
	else
		return AHC_TRUE;
}

#endif //#if (MTP_FUNC == 1)

void AHC_USBGetStates(USB_DETECT_PHASE *pUSBPhase, AHC_USB_GET_PHASE_STAGE sUSBPhaseStage)
{
    void *pusb_obj=NULL;
    //UINT16 cur_phase;

    pusb_obj = AHC_USBGetCurTypeObj(0);
    *pUSBPhase = (USB_DETECT_PHASE)AHC_USBGetPhase(pusb_obj, sUSBPhaseStage);
}

void AHC_USBGetVBUSPinData(UINT8 *bVBusStates)
{
    MMP_UBYTE   ubVbusSR;

    if (USB_IN_DETECT_VBUS_PIN != MMP_GPIO_MAX){
        MMPS_PIO_GetData(USB_IN_DETECT_VBUS_PIN, &ubVbusSR);
    }
    else{
        ubVbusSR = (!USB_IN_DETECT_VBUS_PIN_ACTIVE) & 0x01; //No USB connected
    }
    *bVBusStates = (USB_IN_DETECT_VBUS_PIN_ACTIVE==ubVbusSR) ? 
        USB_IN_DETECT_VBUS_PIN_ACTIVE : (!USB_IN_DETECT_VBUS_PIN_ACTIVE) & 0x01;   
}

void AHC_USBDetectRoutine(void *proutine, UINT8 stage)
{
#define OS_TIMER_ID_UNDEF (0xFF)

    static MMP_ULONG os_timer_id = OS_TIMER_ID_UNDEF;
    struct USB_DETECT_ROUTINE *pusb_det_routine = (struct USB_DETECT_ROUTINE *)proutine;
    MMP_UBYTE ret;

    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    if(stage == 0){ //Stop timer.
        if(os_timer_id != OS_TIMER_ID_UNDEF){
            //OS_CRITICAL_INIT();
            //OS_ENTER_CRITICAL();
            ret = MMPF_OS_StopTimer(os_timer_id, MMPF_OS_TMR_OPT_NONE);
            //OS_EXIT_CRITICAL();

            if (ret) {
                _AHC_USB_ASSERT
            }
            else {
                os_timer_id = OS_TIMER_ID_UNDEF;
            }
        }
    }
    else{ //Start timer.
    //printc("[xxx3]0x%x,%d\r\n", (MMP_ULONG)&AHC_USBDetectHandler, __LINE__);
        os_timer_id = MMPF_OS_StartTimer(pusb_det_routine->timer_interval, MMPF_OS_TMR_OPT_ONE_SHOT, (MMPF_OS_TMR_CALLBACK)pusb_det_routine->pCallback, (void *)pusb_det_routine->pCallbackArg);
        if (os_timer_id >= MMPF_OS_TMRID_MAX) {
            _AHC_USB_ASSERT
        }
    }
}

void AHC_USB_INIT_Detect(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);

    pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
    pusb_det_routine->timer_interval = POLLING_USB_INIT_INTERVAL;//POLLING_VBUS_INTERVAL; //TBD
    pusb_det_routine->timeout = 200; //TBD
    pusb_det_routine->Vbus_states = (!USB_IN_DETECT_VBUS_PIN_ACTIVE) & 0x01;
    pusb_det_routine->pause_detection = 0;
}

void AHC_USB_INIT_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    //printc("0x%x\r\n",MMP_GPIO121);
    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_CHECK_VBUS:
            #ifdef USB_PATH_SELECT_GPIO
            if (USB_PATH_SELECT_GPIO != MMP_GPIO_MAX){
                sRet = MMPS_PIO_SetData(USB_PATH_SELECT_GPIO, USB_PATH_SELECT_SET);
                sRet |= MMPS_PIO_EnableOutputMode(USB_PATH_SELECT_GPIO, MMP_TRUE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }    
            }
            #endif

            #ifdef USB_OUT_PWR_SW_PIN
            if (USB_OUT_PWR_SW_PIN != MMP_GPIO_MAX){
                sRet = MMPS_PIO_SetData(USB_OUT_PWR_SW_PIN, (!USB_OUT_PWR_SW_PIN_ACTIVE) & 0x01);
                sRet |= MMPS_PIO_EnableOutputMode(USB_OUT_PWR_SW_PIN, MMP_TRUE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); } 
            }
            #endif

            #ifdef USB_IN_DETECT_VBUS_PIN
            if (USB_IN_DETECT_VBUS_PIN != MMP_GPIO_MAX){
                sRet = MMPS_PIO_EnableOutputMode(USB_IN_DETECT_VBUS_PIN, MMP_FALSE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); } 
            }
            #endif

            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_INIT_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)
    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    //NOP
}

void AHC_USB_Check_VBUS_Detect(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);
    
    if(1 == pusb_det_routine->pause_detection){
        pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; 
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);        
    }
    else if(USB_IN_DETECT_VBUS_PIN_ACTIVE==pusb_det_routine->Vbus_states){
        /* VBUS Active */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_VBUS_ACTIVE;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //USBH_WAIT_DEV_CONN_CHECK_INTERVAL*USBH_WAIT_DEV_CONN_TIMEOUT_CNT;
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
    else{
        /* VBUS inactive */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_USBID;
        pusb_det_routine->timer_interval = USBH_CHECK_ID_TIMEOUT;
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
}

void AHC_USB_Check_VBUS_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP            
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;            
        case USB_DETECT_PHASE_VBUS_ACTIVE:
            {
                MMP_UBYTE 	i = 0x0, ubTimeOutLimit = 0x10;	// 0x10->0x20 for some PC, USB RESET may delay arrived
                AHC_BOOL 	ubAdapterConnect = AHC_TRUE;

                MMPF_SetUSBChangeMode(USB_DETECT_MODE);
                ubTimeOutLimit = 15;//35;
                for(i = 0x0; i < ubTimeOutLimit ; i++) {
                    if(!MMPF_USB_IsAdapter()) {
                        ubAdapterConnect = MMP_FALSE;
                        break;
                    }
                    AHC_OS_Sleep(100); //To lower the checking frequency, it can be adjusted !
                }

                if(i == ubTimeOutLimit){
                    sRet = MMPF_USB_StopDevice();
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }  
                }
                _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            }
            break;
        case USB_DETECT_PHASE_CHECK_USBID:
            sRet = MMPS_USB_TriggerReadUSBID(0);            //TBD
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); } 
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

extern void USB_Cable_In(UINT8 in);

void AHC_USB_Check_VBUS_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP            
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;                    
        case USB_DETECT_PHASE_VBUS_ACTIVE:
            //SetKeyPadEvent(USB_CABLE_IN); //TBD
            USB_Cable_In(1);
            printc("========== USB cable in. ========== \r\n");
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_USBID:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_USBID_Detect(void *ptype)
{
    MMP_UBYTE ubDevSR;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    sRet = MMPS_USB_ReadUSBID(&ubDevSR);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }    
    
    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);
    
    if(GPIO_LOW == ubDevSR){
        if(USB_IN_DETECT_VBUS_PIN_ACTIVE==pusb_det_routine->Vbus_states){
            /* denied case: exception */
            //_ASSERT_V("OTG_CABLE_VBUS_IN!",__FILE__,__LINE__);
            pusb_det_routine->next_phase = USB_DETECT_PHASE_NG;
            pusb_det_routine->timer_interval = 0; //TBD
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
        else{
            pusb_det_routine->next_phase = USB_DETECT_PHASE_OTG_SESSION;
            pusb_det_routine->timer_interval = WAIT_OTG_SESSION_INTERVAL;
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
    }
    else{
        /* USB_ID HIGH: NC or B Device */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL;
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
}

void AHC_USB_Check_USBID_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_OTG_SESSION:
            /* Enable PWR SW, VBus power to device */
            #ifdef USB_OUT_PWR_SW_PIN
            if (USB_OUT_PWR_SW_PIN != MMP_GPIO_MAX) {
                sRet = MMPS_PIO_SetData(USB_OUT_PWR_SW_PIN, USB_OUT_PWR_SW_PIN_ACTIVE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
            }
            #endif
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_NG:
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_USBID_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    //NOP
    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
}

#if USB_DETECTION_CUSTOM
void AHC_USB_Check_VBUS_Custom_Detect(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);
    
    if((USB_IN_DETECT_VBUS_PIN_ACTIVE == pusb_det_routine->Vbus_states) ){
        /* VBUS Active */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_USBID;
        pusb_det_routine->timer_interval = USBH_CHECK_ID_TIMEOUT; //USBH_WAIT_DEV_CONN_CHECK_INTERVAL*USBH_WAIT_DEV_CONN_TIMEOUT_CNT;
        pusb_det_routine->timeout = (pusb_det_routine->timeout > 0) ? pusb_det_routine->timeout - 1 : pusb_det_routine->timeout;       
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
    else{
        /* VBUS inactive */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL;
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
}

void AHC_USB_Check_VBUS_Custom_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_USBID:
            sRet = MMPS_USB_TriggerReadUSBID(0);            //TBD
            if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }  
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_VBUS_Custom_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_USBID:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_USBID_Custom_Detect(void *ptype)
{
    MMP_UBYTE ubDevSR;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    sRet = MMPS_USB_ReadUSBID(&ubDevSR);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
    
    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);

    if(GPIO_LOW == ubDevSR){
        if(USB_IN_DETECT_VBUS_PIN_ACTIVE==pusb_det_routine->Vbus_states && (pusb_det_routine->timeout != 0)){
            pusb_det_routine->next_phase = USB_DETECT_PHASE_OTG_SESSION;
            pusb_det_routine->timer_interval = WAIT_OTG_SESSION_INTERVAL;
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
        else{
            pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
            pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL;
            pusb_det_routine->timeout = pusb_det_routine->timeout ? pusb_det_routine->timeout - 1 : 0;       
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
    }
    else{
        if(USB_IN_DETECT_VBUS_PIN_ACTIVE==pusb_det_routine->Vbus_states){
            /* VBUS Active and ID=High*/
            pusb_det_routine->next_phase = USB_DETECT_PHASE_VBUS_ACTIVE;
            pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //USBH_WAIT_DEV_CONN_CHECK_INTERVAL*USBH_WAIT_DEV_CONN_TIMEOUT_CNT;
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
        else{
            /* USB_ID HIGH: NC or B Device */
            pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
            pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL;
            pusb_det_routine->timeout = pusb_det_routine->timeout ? pusb_det_routine->timeout - 1 : 0;                   
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
    }
}

void AHC_USB_Check_USBID_Custom_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;
    MMP_ERR sRet = MMP_ERR_NONE;
    
    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_OTG_SESSION:
            /* Enable PWR SW, VBus power to device */
            #ifdef USB_OUT_PWR_SW_PIN
            if (USB_OUT_PWR_SW_PIN != MMP_GPIO_MAX) {
                sRet = MMPS_PIO_SetData(USB_OUT_PWR_SW_PIN, USB_OUT_PWR_SW_PIN_ACTIVE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }    
            }
            #endif
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_VBUS_ACTIVE:
            {
                MMP_UBYTE 	i = 0x0, ubTimeOutLimit = 0x10;	// 0x10->0x20 for some PC, USB RESET may delay arrived
                AHC_BOOL 	ubAdapterConnect = AHC_TRUE;

                MMPF_SetUSBChangeMode(USB_DETECT_MODE);
                ubTimeOutLimit = 15;//35;
                for(i = 0x0; i < ubTimeOutLimit ; i++) {
                    if(!MMPF_USB_IsAdapter()) {
                        ubAdapterConnect = MMP_FALSE;
                        break;
                    }
                    AHC_OS_Sleep(100); //To lower the checking frequency, it can be adjusted !
                }

                if(i == ubTimeOutLimit){
                    sRet = MMPF_USB_StopDevice();
                    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }    
                }
                _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            }
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_USBID_Custom_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)
    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_OTG_SESSION:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_VBUS_ACTIVE:
            USB_Cable_In(1);
            printc("========== USB cable in. ========== \r\n");
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_Connect_Custom_Detect(void *ptype)
{
    MMP_BOOL    bStatus = MMP_TRUE;
    MMP_BOOL    bRearCamValid = AHC_FALSE;
    MMP_ERR sRet = MMP_ERR_NONE;
   
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    sRet = MMPS_USB_GetDevConnSts(&bStatus);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet);}    
    
    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);    
    
    bRearCamValid = ((MMP_TRUE == bStatus) && (USB_IN_DETECT_VBUS_PIN_ACTIVE == pusb_det_routine->Vbus_states)) ? AHC_TRUE : AHC_FALSE;
    
#if (UVC_HOST_VIDEO_ENABLE)
    {
        UINT32 ulUVCHostErrorType=0;
        AHC_HostUVCVideoReadErrorType(&ulUVCHostErrorType);
        if((ulUVCHostErrorType & UVC_HOST_ERROR_EP0_TIMEOUT) || 
            (ulUVCHostErrorType & UVC_HOST_ERROR_FRM_RX_LOST_TH_2) ||
            (ulUVCHostErrorType & UVC_HOST_ERROR_DISCONNECTED)){
            /* invalid. Connection is lost.*/
            bRearCamValid = AHC_FALSE;
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
        else if((ulUVCHostErrorType & UVC_HOST_ERROR_TRAN_TIMEOUT) || 
            (ulUVCHostErrorType & UVC_HOST_ERROR_FRM_RX_LOST) ||
            (ulUVCHostErrorType & UVC_HOST_ERROR_SEQ_NO_LOST)){
            //Force I
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
    }
#endif

    if(AHC_TRUE == bRearCamValid){
        /* valid rear cam */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_REAR_CAM;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //TBD
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
    else{
        switch(pusb_det_routine->cur_phase){
            case USB_DETECT_PHASE_CHECK_CONN:
                pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
                pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //TBD            
                _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
                break;
            case USB_DETECT_PHASE_REAR_CAM:
                /*Go to USB_DETECT_PHASE_REAR_CAM_DISCONN states to check VBUS pin.*/
                pusb_det_routine->next_phase = USB_DETECT_PHASE_REAR_CAM_DISCONN;
                pusb_det_routine->timer_interval = 1500; //Wait until VBUS stable
                _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
                break;
            default:
                _AHC_USB_ASSERT
                break;
        }
                
        if(pusb_det_routine->cur_phase == USB_DETECT_PHASE_CHECK_CONN){
            pusb_det_routine->timeout = pusb_det_routine->timeout ? pusb_det_routine->timeout - 1 : 0;
        }
        else{
        }
        
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
}

void AHC_USB_RearCam_Disconn_Custom_Detect(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
    pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL;
    pusb_det_routine->timeout = pusb_det_routine->timeout_init_val;   
    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
}

void AHC_USB_RearCam_Disconn_Custom_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_CHECK_VBUS:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_RearCam_Disconn_Custom_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);
    
    if(USB_IN_DETECT_VBUS_PIN_ACTIVE == pusb_det_routine->Vbus_states){
        //NOP
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);        
    }
    else{
        USB_Cable_In(0);
        printc("========== USB cable out. ========== \r\n");
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
}


#endif

void AHC_USB_OTG_Session_Detect(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_CONN;//USB_DETECT_PHASE_OTG_SESSION;
    pusb_det_routine->timer_interval = WAIT_CHECK_BDEVICE_CONN_INTERVAL;

    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
}

void AHC_USB_OTG_Session_Exec(void *ptype)
{
#if (SUPPORT_USB_HOST_FUNC)
    MMP_ERR sRet = MMP_ERR_NONE;
#endif
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)
#if (SUPPORT_USB_HOST_FUNC)
    sRet = MMPS_USB_AdjustVref(gususbh_vref);
    sRet |= MMPS_USB_AdjustBiasCurrent(gususbh_bias_current);
    sRet |= MMPS_USB_AdjustSignal(gususbh_tx_cur, gususbh_tx_cur_plus, gususbh_sq_level);
    sRet |= MMPS_USB_StartOTGSession(0);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }    
#endif
    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
}

void AHC_USB_OTG_Session_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    //NOP
    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
}

void AHC_USB_Check_Connect_Detect(void *ptype)
{
    MMP_BOOL bStatus = MMP_TRUE;
    MMP_ERR sRet = MMP_ERR_NONE;    
    
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    static MMP_ULONG ulOTGDisconnectDelay = WAIT_CHECK_BDEVICE_DISCONN_DELAY_CNT;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    sRet = MMPS_USB_GetDevConnSts(&bStatus);
    if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }    
    
    if(1 == pusb_det_routine->pause_detection){
        pusb_det_routine->next_phase = pusb_det_routine->cur_phase;//USB_DETECT_PHASE_REAR_CAM;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //TBD
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);        
    }
    else if(MMP_TRUE==bStatus){
        /* valid rear cam */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_REAR_CAM;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //TBD

#if (UVC_HOST_VIDEO_ENABLE)
        {
            UINT32 ulUVCHostErrorType=0;
            AHC_HostUVCVideoReadErrorType(&ulUVCHostErrorType);
            if((ulUVCHostErrorType & UVC_HOST_ERROR_EP0_TIMEOUT) || 
                (ulUVCHostErrorType & UVC_HOST_ERROR_FRM_RX_LOST_TH_2) ||
                (ulUVCHostErrorType & UVC_HOST_ERROR_DISCONNECTED)){
                /* invalid. Connection is lost.*/
                pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
                pusb_det_routine->timer_interval = WAIT_BDEVICE_SHUTDOWN_INTERVAL;
                _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            }
            else if((ulUVCHostErrorType & UVC_HOST_ERROR_TRAN_TIMEOUT) || 
                (ulUVCHostErrorType & UVC_HOST_ERROR_FRM_RX_LOST) ||
                (ulUVCHostErrorType & UVC_HOST_ERROR_SEQ_NO_LOST)){
                //Force I
                _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            }
        }
#endif
        ulOTGDisconnectDelay = WAIT_CHECK_BDEVICE_DISCONN_DELAY_CNT;

        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
    else{

        if(--ulOTGDisconnectDelay > 0){
            //Wait until countdown to zero.
            pusb_det_routine->next_phase = pusb_det_routine->cur_phase;
            pusb_det_routine->timer_interval = WAIT_CHECK_BDEVICE_CONN_INTERVAL;
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);            
        }
        else{
            /* invalid / timeout.  Only OTG cable but no device connected.*/
            pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
            pusb_det_routine->timer_interval = WAIT_BDEVICE_SHUTDOWN_INTERVAL;
            ulOTGDisconnectDelay = WAIT_CHECK_BDEVICE_DISCONN_DELAY_CNT;
            
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
        }
    }
}

void AHC_USB_Check_Connect_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;
    static AHC_BOOL bUVCDeviceEnum = 0;
#if (UVC_HOST_VIDEO_ENABLE)
    UINT32 ulUVCHostErrorType=0;
#endif
    MMP_ERR sRet = MMP_ERR_NONE;
    AHC_BOOL ahcRet = AHC_TRUE;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

#if (UVC_HOST_VIDEO_ENABLE)
    AHC_HostUVCVideoReadErrorType(&ulUVCHostErrorType);
#endif

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_REAR_CAM:
            #if UVC_HOST_VIDEO_ENABLE
            if(bUVCDeviceEnum == 0){
                AHC_HostUVCVideoClearError((UINT32)(-1)); //Cleare all error flag.
                ahcRet = AHC_HostUVCVideoEnumDevice(AHC_TRUE);
                if(ahcRet == AHC_TRUE){
                    bUVCDeviceEnum = 1;
                }
                else{ //Enum fail.
                    pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
                    pusb_det_routine->timer_interval = WAIT_BDEVICE_SHUTDOWN_INTERVAL;                    
                    //Disable rear cam power.
#ifdef USB_OUT_PWR_SW_PIN
                    if (USB_OUT_PWR_SW_PIN != MMP_GPIO_MAX){
                        sRet = MMPS_PIO_SetData(USB_OUT_PWR_SW_PIN, (!USB_OUT_PWR_SW_PIN_ACTIVE) & 0x01);
                        if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
                    }
#endif                    
                    printc("Enum fail\r\n");
                }
            }
            else{
                if((ulUVCHostErrorType & UVC_HOST_ERROR_TRAN_TIMEOUT) || 
                    (ulUVCHostErrorType & UVC_HOST_ERROR_FRM_RX_LOST) ||
                    (ulUVCHostErrorType & UVC_HOST_ERROR_SEQ_NO_LOST)){
                    //Force I
                    ahcRet = AHC_HostUVCForceIFrame();
                    if(ahcRet != AHC_TRUE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
                    
                    //Clear error flag
                    AHC_HostUVCVideoClearError(UVC_HOST_ERROR_TRAN_TIMEOUT | UVC_HOST_ERROR_FRM_RX_LOST | UVC_HOST_ERROR_SEQ_NO_LOST);
                    _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
                }
            }
            #endif
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
#if USB_DETECTION_CUSTOM
        case USB_DETECT_PHASE_REAR_CAM_DISCONN:
#endif            
            bUVCDeviceEnum = 0;
            #if UVC_HOST_VIDEO_ENABLE
            ahcRet = AHC_HostUVCVideoEnumDevice(AHC_FALSE);
            if(ahcRet == AHC_FALSE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); }

            if((ulUVCHostErrorType & UVC_HOST_ERROR_EP0_TIMEOUT) || 
               (ulUVCHostErrorType & UVC_HOST_ERROR_FRM_RX_LOST_TH_2) ||
               (ulUVCHostErrorType & UVC_HOST_ERROR_DISCONNECTED)){
                //EP0 timeout happens only in idle states.
                /* forced to clear connect status because of disconnected, not from USB_OTG_ISR (PHY) */
                sRet = MMPS_USB_SetDevConnSts(MMP_FALSE);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
                
                /* reset USB */
                sRet = MMPS_USB_DisconnectDevice();
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
            }
            AHC_HostUVCVideoClearError((UINT32)(-1)); //Cleare all error flag.
            #endif

            //Disable rear cam power.
            #ifdef USB_OUT_PWR_SW_PIN
            if (USB_OUT_PWR_SW_PIN != MMP_GPIO_MAX){
                sRet = MMPS_PIO_SetData(USB_OUT_PWR_SW_PIN, (!USB_OUT_PWR_SW_PIN_ACTIVE) & 0x01);
                if(sRet != MMP_ERR_NONE){ AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); }   
            }
            #endif

            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;

        case USB_DETECT_PHASE_CHECK_CONN:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            bUVCDeviceEnum = 0;
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Check_Connect_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_REAR_CAM:
            //TBD
            //Rear cam plugs in.
            USB_BDevice_In(1);
            printc("========== Rearcam in. ========== \r\n");
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
#if USB_DETECTION_CUSTOM
        case USB_DETECT_PHASE_REAR_CAM_DISCONN:
#endif                        
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            //NOP
            break;
        case USB_DETECT_PHASE_CHECK_CONN:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;            
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_VBUS_Active_Detect(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    AHC_USBGetVBUSPinData(&pusb_det_routine->Vbus_states);

    if(USB_IN_DETECT_VBUS_PIN_ACTIVE==pusb_det_routine->Vbus_states){
        /* VBUS active */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_VBUS_ACTIVE;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL; //TBD
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
    else{
        /* VBUS inactive */
        pusb_det_routine->next_phase = USB_DETECT_PHASE_CHECK_VBUS;
        pusb_det_routine->timer_interval = POLLING_VBUS_INTERVAL;
        _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
    }
}

void AHC_USB_VBUS_Active_Exec(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_VBUS_ACTIVE:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
            //MMPS_USB_TriggerReadUSBID(0);
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_VBUS_Active_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_VBUS_ACTIVE:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
            //SetKeyPadEvent(USB_CABLE_OUT); //TBD
            USB_Cable_In(0);
            printc("========== USB cable out. ========== \r\n");
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_Rearcam_Notify(void *ptype)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    switch(pusb_det_routine->next_phase){
        case USB_DETECT_PHASE_REAR_CAM:
            //NOP
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        case USB_DETECT_PHASE_CHECK_VBUS:
#if USB_DETECTION_CUSTOM
        case USB_DETECT_PHASE_REAR_CAM_DISCONN:
#endif                                    
            //To do here.
            //Rear cam plugs out.
            USB_BDevice_In(0);
            printc("========== Rearcam out. ========== \r\n");
            _AHC_USB_PRINTC("%s,%d\r\n", __func__, __LINE__);
            break;
        default:
            _AHC_USB_ASSERT
            break;
    }
}

void AHC_USB_SubRoutine(void *ptype, void *ptmr, void *parg, UINT8 stage)
{
    struct USB_DETECT_TYPE *pdet_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pdet_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    if(stage == 0){
        pusb_det_routine->pCallback = ptmr;
        pusb_det_routine->pCallbackArg = parg;
    }
    pusb_det_routine->usb_routine((void *)pusb_det_routine, stage);
}

UINT32 AHC_USBGetTypeNum(void)
{
    return sizeof(_usb_det_type) / sizeof(struct USB_DETECT_TYPE);
}

void *AHC_USBGetCurTypeObj(UINT16 type_num)
{
    struct USB_DETECT_TYPE *pusb_det_type;

    pusb_det_type = (struct USB_DETECT_TYPE *)(&_usb_det_type);
    return (void *)(pusb_det_type + type_num);
}

UINT16 AHC_USBGetCurTypePhase(void *pdet_type)
{
    struct USB_DETECT_TYPE *pusb_det_type = (struct USB_DETECT_TYPE *)pdet_type;

    _AHC_USB_CHECK_POINTER_VALID(pusb_det_type)
    return pusb_det_type->usb_phase;
}

void AHC_USBRoutine(void *ptype, void *ptmr, void *parg, UINT8 stage)
{
    struct USB_DETECT_TYPE *pusb_det_type = (struct USB_DETECT_TYPE *)ptype;

    _AHC_USB_CHECK_POINTER_VALID(pusb_det_type)
    pusb_det_type->routine_func((void *)pusb_det_type, ptmr, parg, stage);
}

void AHC_USB_Do(void *ptype)
{
    struct USB_DETECT_TYPE *pusb_det_type = (struct USB_DETECT_TYPE *)ptype;

    _AHC_USB_CHECK_POINTER_VALID(pusb_det_type)

    pusb_det_type->detect_func((void *)pusb_det_type);
    if(pusb_det_type->exec_func){
        pusb_det_type->exec_func((void *)pusb_det_type);
    }
    if(pusb_det_type->notify_func){
        pusb_det_type->notify_func((void *)pusb_det_type);
    }
}

void AHC_USBSetPhase(void *ptype, UINT16 phase)
{
    struct USB_DETECT_TYPE *pusb_det_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pusb_det_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    pusb_det_routine->cur_phase = (USB_DETECT_PHASE)phase;
}

UINT16 AHC_USBGetPhase(void *ptype, UINT8 cur_or_next)
{
    struct USB_DETECT_TYPE *pusb_det_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pusb_det_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)

    if(AHC_USB_GET_PHASE_NEXT==cur_or_next){ // 1: next
        return (UINT16)pusb_det_routine->next_phase;
    }

    // 0: current
    return (UINT16)pusb_det_routine->cur_phase;
}

UINT8 AHC_USBGetVBUS(void *ptype)
{
    struct USB_DETECT_TYPE *pusb_det_type = (struct USB_DETECT_TYPE *)ptype;
    struct USB_DETECT_ROUTINE *pusb_det_routine;

    pusb_det_routine = (struct USB_DETECT_ROUTINE *)pusb_det_type->usb_detect_routine;
    _AHC_USB_CHECK_POINTER_VALID(pusb_det_routine)
        
    return pusb_det_routine->Vbus_states;
}

void AHC_USBDetectSetPriority(AHC_BOOL bHighPriority)
{
    gbAHC_USBDetectionHighPriority = bHighPriority;
}

void AHC_USBDetectTimerCB(void *ptmr, void *parg)
{
    extern MMPF_OS_MQID         HighTask_QId;
    extern MMPF_OS_MQID         HighTaskWork_QId;

    //printc("%s,%d\r\n", __func__, __LINE__);
    if(AHC_TRUE == gbAHC_USBDetectionHighPriority){
        MMPF_OS_PutMessage(HighTaskWork_QId, (void *)(&AHC_USBDetectHandler));
    }
    else{
        MMPF_OS_PutMessage(HighTask_QId, (void *)(&AHC_USBDetectHandler));
    }    
}

void AHC_USBDetectHandler(void)
{
    UINT32 loop=0, total_num=0;
    UINT16 cur_phase=0;
    void *pusb_obj=NULL;
    //printc("%s,%d\r\n", __func__, __LINE__);

#if (UVC_HOST_VIDEO_ENABLE)
    {
        // Only once after power-on
        static AHC_BOOL isFirstSetting = AHC_TRUE;
        
        if (isFirstSetting) {
            MMP_ERR sRet = MMP_ERR_NONE;
            
            isFirstSetting = AHC_FALSE;
            sRet = MMPS_USB_SetCheckDevAliveCFG(CHECK_EP0_TIMEOUT_INTERVAL,(void *)AHC_HostUVCVideoSetEp0TimeoutCB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}
            sRet = MMPS_USB_SetTranTimeoutCFG(CHECK_TRAN_TIMEOUT_INTERVAL, (void *)AHC_HostUVCVideoSetTranTimeoutCB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}
            sRet = MMPS_USB_SetFrmRxTimeoutCFG(CHECK_FRM_RX_LOST_TH, CHECK_FRM_RX_FIRST_ADD_GAP, (void *)AHC_HostUVCVideoSetFrmRxTimeoutCB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}            
            sRet = MMPS_USB_SetFrmRxTimeout2CFG(CHECK_FRM_RX_LOST_TH_2,    (void *)AHC_HostUVCVideoSetFrmRxTimeout2CB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}            
            sRet = MMPS_USB_SetFrmSeqNoLostCFG(CHECK_FRM_SEQ_NO_LOST_TH,   (void *)AHC_HostUVCVideoSetFrmSeqNoLostCB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}            
            sRet = MMPS_USB_SetDropRxFrmCFG(CHECK_FRM_RX_DROP_TOTAL_CNT, CHECK_FRM_RX_DROP_CONTI_CNT, (void *)AHC_HostUVCVideoSetFrmRxDropCB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}            
            sRet = MMPS_USB_RegDetectCallBack(NULL, NULL, NULL, (void *)AHC_HostUVCVideoDevDisconnectedCB);
            if(sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); /*return;*/}            
        }
    }
#endif

    total_num = AHC_USBGetTypeNum();

    for(loop=0; loop < total_num; ++loop){
        pusb_obj = AHC_USBGetCurTypeObj(loop);
        cur_phase = AHC_USBGetPhase(pusb_obj, AHC_USB_GET_PHASE_CURRENT);
        //printc("%s,%d, 0x%x\r\n", __func__, __LINE__, (MMP_ULONG)pusb_obj);
        if(cur_phase == AHC_USBGetCurTypePhase(pusb_obj)){
            //printc("[xxx2]0x%x,%d\r\n", (MMP_ULONG)&AHC_USBDetectTimerCB, __LINE__);
            AHC_USBRoutine(pusb_obj, (void *)&AHC_USBDetectTimerCB, NULL, 0);
            AHC_USB_Do(pusb_obj);
            AHC_USBRoutine(pusb_obj, NULL, NULL, 1);
            AHC_USBSetPhase(pusb_obj, AHC_USBGetPhase(pusb_obj, AHC_USB_GET_PHASE_NEXT));
            break;
        }
    }
}


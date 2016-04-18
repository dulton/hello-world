//==============================================================================
//
//  File        : version.c
//  Description : Version Control Function of Firmware Core
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"
#include "lib_retina.h"
#include "mmpf_uart.h"
#include "version.h"

//==============================================================================
//
//                              MACRO FUNCTIONS
//
//==============================================================================

#define DVR_CFG_DUMP_B(cfg)	            \
{                                       \
    RTNA_DBG_Str(0, #cfg":");           \
    RTNA_DBG_Byte(0, ##cfg);            \
    RTNA_DBG_Str(0, "\r\n");            \
}

#define DVR_CFG_DUMP_W(cfg)	            \
{                                       \
    RTNA_DBG_Str(0, #cfg":");           \
    RTNA_DBG_Short(0, ##cfg);           \
    RTNA_DBG_Str(0, "\r\n");            \
}

#define DVR_CFG_DUMP_L(cfg)             \
{                                       \
    RTNA_DBG_Str(0, #cfg":");           \
    RTNA_DBG_Long(0, ##cfg);            \
    RTNA_DBG_Str(0, "\r\n");            \
}

#define DVR_CFG_DUMP_S(cfg)             \
{                                       \
    RTNA_DBG_Str(0, #cfg":"##cfg"\r\n");\
}

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================
//------------------------------------------------------------------------------
//  Function    : DVR_Version
//  Description :
//------------------------------------------------------------------------------
/** @brief The function returns the current version of firmware core

The function returns the version of firmware
@param[out] major the major number of version.
@param[out] minor the minor number of version.
@param[out] patch the patch number of version.
@return none.
*/
void DVR_Version(MMP_USHORT *major, MMP_USHORT *minor, MMP_USHORT *patch)
{
    *major = VERSION_MAJOR;
    *minor = VERSION_MINOR;
    *patch = VERSION_PATCH;
}

//------------------------------------------------------------------------------
//  Function    : DVR_DumpConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief The function dumps the all configurations of firmware core

The function dumps the all configurations in firmware
@return none.
*/
void DVR_DumpConfig(void)
{
    DVR_DumpSystemCfg();
    DVR_DumpUartCfg();
    DVR_DumpFlowCtrlCfg();
    DVR_DumpMMUCfg();
    DVR_DumpSensorCfg();
    DVR_DumpISPCfg();
    DVR_DumpDisplayCfg();
    DVR_DumpVidPlayCfg();
    DVR_DumpVidRecCfg();
    DVR_DumpStorageCfg();
    DVR_DumpUSBCfg();
    DVR_DumpDSCCfg();
    DVR_DumpAudioCfg();
}

/* System_Config */
void DVR_DumpSystemCfg(void)
{
    #ifdef CHIP
    DVR_CFG_DUMP_B(CHIP)
    #endif
    #ifdef CPU_ID
    DVR_CFG_DUMP_B(CPU_ID)
    #endif
    #ifdef MCI_READ_QUEUE_4
    DVR_CFG_DUMP_B(MCI_READ_QUEUE_4)
    #endif
    #ifdef AUTO_DRAM_LOCKCORE
    DVR_CFG_DUMP_B(AUTO_DRAM_LOCKCORE)
    #endif
}

/* UART_Config */
void DVR_DumpUartCfg(void)
{
    #ifdef DEBUG_UART_NUM
    DVR_CFG_DUMP_B(DEBUG_UART_NUM)
    #endif
    #ifdef DEBUG_UART_PAD
    DVR_CFG_DUMP_B(DEBUG_UART_PAD)
    #endif
    #ifdef DBG_LEVEL
    DVR_CFG_DUMP_B(DBG_LEVEL)
    #endif
}

/* FlowControl_Config */
void DVR_DumpFlowCtrlCfg(void)
{
    #ifdef SUPPORT_LDC_RECD
    DVR_CFG_DUMP_B(SUPPORT_LDC_RECD)
    #endif
}

/* MMU_Table_Config */
void DVR_DumpMMUCfg(void)
{
    #ifdef MMU_TRANSLATION_TABLE_ADDR
    DVR_CFG_DUMP_L(MMU_TRANSLATION_TABLE_ADDR)
    #endif
    #ifdef MMU_COARSEPAGE_TABLE_ADDR
    DVR_CFG_DUMP_L(MMU_COARSEPAGE_TABLE_ADDR)
    #endif
    #ifdef RSVD_HEAP_SIZE
    DVR_CFG_DUMP_L(RSVD_HEAP_SIZE)
    #endif
}

/* Sensor_Config */
void DVR_DumpSensorCfg(void)
{
    #ifdef TOTAL_SENSOR_NUMBER
    DVR_CFG_DUMP_B(TOTAL_SENSOR_NUMBER)
    #endif
}

/* ISP_Config */
void DVR_DumpISPCfg(void)
{
    #ifdef ISP_USE_TASK_DO3A
    DVR_CFG_DUMP_B(ISP_USE_TASK_DO3A)
    #endif
}

/* Display_Config */
void DVR_DumpDisplayCfg(void)
{
    #ifdef LCD_IF
    DVR_CFG_DUMP_B(LCD_IF)
    #endif
    #ifdef HDMI_OUTPUT_EN
    DVR_CFG_DUMP_B(HDMI_OUTPUT_EN)
    #endif
}

/* Video_Play_Config */
void DVR_DumpVidPlayCfg(void)
{
    #ifdef ROTATE_DMA_SUPPORT
    DVR_CFG_DUMP_B(ROTATE_DMA_SUPPORT)
    #endif
    #ifdef GRAPHIC_SCALER_SUPPORT
    DVR_CFG_DUMP_B(GRAPHIC_SCALER_SUPPORT)
    #endif
    #ifdef VIDEO_BGPARSER_EN
    DVR_CFG_DUMP_B(VIDEO_BGPARSER_EN)
    #endif
    #ifdef CHANGE_AV_SPEED
    DVR_CFG_DUMP_B(CHANGE_AV_SPEED)
    #endif
    #ifdef MJPG_DEC_SUPPORT
    DVR_CFG_DUMP_B(MJPG_DEC_SUPPORT)
    #endif
    #ifdef VIDEO_DEC_TO_MJPEG
    DVR_CFG_DUMP_B(VIDEO_DEC_TO_MJPEG)
    #endif
}

/* Video_Record_Config */
void DVR_DumpVidRecCfg(void)
{
    #ifdef VR_AVSYNC_METHOD
    DVR_CFG_DUMP_B(VR_AVSYNC_METHOD)
    #endif
    #ifdef VR_ENC_EARLY_START
    DVR_CFG_DUMP_B(VR_ENC_EARLY_START)
    #endif
    #ifdef VR_SINGLE_CUR_FRAME
    DVR_CFG_DUMP_B(VR_SINGLE_CUR_FRAME)
    #endif
    #ifdef SHARE_REF_GEN_BUF
    DVR_CFG_DUMP_B(SHARE_REF_GEN_BUF)
    #endif
    #ifdef SUPPORT_B_FRAME_ENC
    DVR_CFG_DUMP_B(SUPPORT_B_FRAME_ENC)
    #endif
    #ifdef MAX_B_FRAME_NUMS
    DVR_CFG_DUMP_B(MAX_B_FRAME_NUMS)
    #endif
    #ifdef AVI_IDIT_CHUNK_EN
    DVR_CFG_DUMP_B(AVI_IDIT_CHUNK_EN)
    #endif
    #ifdef AVI_ISFT_CHUNK_EN
    DVR_CFG_DUMP_B(AVI_ISFT_CHUNK_EN)
    #endif
    #ifdef AVI_ISFT_DATA_STR
    DVR_CFG_DUMP_S(AVI_ISFT_DATA_STR)
    #endif
    #ifdef SUPPORT_MDTC
    DVR_CFG_DUMP_B(SUPPORT_MDTC)
    #endif
    #ifdef EMERGENTRECD_SUPPORT
    DVR_CFG_DUMP_B(EMERGENTRECD_SUPPORT)
    #endif
    #ifdef DUALENC_SUPPORT
    DVR_CFG_DUMP_B(DUALENC_SUPPORT)
    #endif
    #ifdef UVC_VIDRECD_SUPPORT
    DVR_CFG_DUMP_B(UVC_VIDRECD_SUPPORT)
    #endif
    #ifdef SUPPORT_VR_WIFI_STREAM
    DVR_CFG_DUMP_B(SUPPORT_VR_WIFI_STREAM)
    #endif
    #ifdef VIDRECD_MULTI_TRACK
    DVR_CFG_DUMP_B(VIDRECD_MULTI_TRACK)
    #endif
}

/* Storage_Config */
void DVR_DumpStorageCfg(void)
{
    #ifdef FS_LIB
    DVR_CFG_DUMP_B(FS_LIB)
    #endif
    #ifdef FS_INPUT_ENCODE
    DVR_CFG_DUMP_B(FS_INPUT_ENCODE)
    #endif
    #ifdef SF_PIN_PAD_NUM
    DVR_CFG_DUMP_B(SF_PIN_PAD_NUM)
    #endif
    #ifdef EN_CARD_DETECT
    DVR_CFG_DUMP_B(EN_CARD_DETECT)
    #endif
    #ifdef EN_CARD_WRITEPROTECT
    DVR_CFG_DUMP_B(EN_CARD_WRITEPROTECT)
    #endif
    #ifdef EN_CARD_PWRCTL
    DVR_CFG_DUMP_B(EN_CARD_PWRCTL)
    #endif
    #ifdef USING_SD_IF
    DVR_CFG_DUMP_B(USING_SD_IF)
    #endif
    #ifdef USING_SM_IF
    DVR_CFG_DUMP_B(USING_SM_IF)
    #endif
    #ifdef USING_MEMORY_IF
    DVR_CFG_DUMP_B(USING_MEMORY_IF)
    #endif
    #ifdef USING_SF_IF
    DVR_CFG_DUMP_B(USING_SF_IF)
    #endif
    #ifdef USING_DUAL_IF
    DVR_CFG_DUMP_B(USING_DUAL_IF)
    #endif
    #ifdef FS_MAX_UNITS
    DVR_CFG_DUMP_B(FS_MAX_UNITS)
    #endif
    #ifdef MSDC_MAX_UNITS
    DVR_CFG_DUMP_B(MSDC_MAX_UNITS)
    #endif
    #ifdef FS_1ST_DEV
    DVR_CFG_DUMP_S(FS_1ST_DEV)
    #endif
    #ifdef FS_2ND_DEV
    DVR_CFG_DUMP_S(FS_2ND_DEV)
    #endif
    #ifdef FS_3RD_DEV
    DVR_CFG_DUMP_S(FS_3RD_DEV)
    #endif
    #ifdef MSDC_1ST_DEV
    DVR_CFG_DUMP_S(MSDC_1ST_DEV)
    #endif
    #ifdef MSDC_2ND_DEV
    DVR_CFG_DUMP_S(MSDC_2ND_DEV)
    #endif
    #ifdef MSDC_3RD_DEV
    DVR_CFG_DUMP_S(MSDC_3RD_DEV)
    #endif
}

/* USB_Config */
void DVR_DumpUSBCfg(void)
{
    #ifdef SUPPORT_PCCAM_FUNC
    DVR_CFG_DUMP_B(SUPPORT_PCCAM_FUNC)
    #endif
    #ifdef SUPPORT_PCSYNC_FUNC
    DVR_CFG_DUMP_B(SUPPORT_PCSYNC_FUNC)
    #endif
    #ifdef SUPPORT_MTP_FUNC
    DVR_CFG_DUMP_B(SUPPORT_MTP_FUNC)
    #endif
    #ifdef SUPPORT_DPS_FUNC
    DVR_CFG_DUMP_B(SUPPORT_DPS_FUNC)
    #endif
    #ifdef MSDC_WR_FLOW_TEST_EN
    DVR_CFG_DUMP_B(MSDC_WR_FLOW_TEST_EN)
    #endif
    #ifdef SUPPORT_UVC_FUNC
    DVR_CFG_DUMP_B(SUPPORT_UVC_FUNC)
    #endif
    #ifdef SUPPORT_UAC
    DVR_CFG_DUMP_B(SUPPORT_UAC)
    #endif
    #ifdef FPS_CTL
    DVR_CFG_DUMP_B(FPS_CTL)
    #endif
}

/* DSC_Config */
void DVR_DumpDSCCfg(void)
{
    #ifdef SUPPORT_FDTC
    DVR_CFG_DUMP_B(SUPPORT_FDTC)
    #endif
    #ifdef FDTC_AIT_SDTC
    DVR_CFG_DUMP_B(FDTC_AIT_SDTC)
    #endif
}

/* Audio_Config */
void DVR_DumpAudioCfg(void)
{
    #ifdef DEFAULT_DAC_DIGITAL_GAIN
    DVR_CFG_DUMP_B(DEFAULT_DAC_DIGITAL_GAIN)
    #endif
    #ifdef DEFAULT_DAC_ANALOG_GAIN
    DVR_CFG_DUMP_B(DEFAULT_DAC_ANALOG_GAIN)
    #endif
    #ifdef DEFAULT_DAC_LINEOUT_GAIN
    DVR_CFG_DUMP_B(DEFAULT_DAC_LINEOUT_GAIN)
    #endif
    #ifdef DEFAULT_ADC_DIGITAL_GAIN
    DVR_CFG_DUMP_B(DEFAULT_ADC_DIGITAL_GAIN)
    #endif
    #ifdef DEFAULT_ADC_ANALOG_GAIN
    DVR_CFG_DUMP_B(DEFAULT_ADC_ANALOG_GAIN)
    #endif
    #ifdef WNR_EN
    DVR_CFG_DUMP_B(WNR_EN)
    #endif
    #ifdef NR_EN
    DVR_CFG_DUMP_B(NR_EN)
    #endif
    #ifdef AGC_SUPPORT
    DVR_CFG_DUMP_B(AGC_SUPPORT)
    #endif
    #ifdef EXT_CODEC_SUPPORT
    DVR_CFG_DUMP_B(EXT_CODEC_SUPPORT)
    #endif
    #ifdef SBC_SUPPORT
    DVR_CFG_DUMP_B(SBC_SUPPORT)
    #endif
    #ifdef SRC_SUPPORT
    DVR_CFG_DUMP_B(SRC_SUPPORT)
    #endif
    #ifdef VIDEO_SRC_SUPPORT
    DVR_CFG_DUMP_B(VIDEO_SRC_SUPPORT)
    #endif
    #ifdef PCM_TO_DNSE_EN
    DVR_CFG_DUMP_B(PCM_TO_DNSE_EN)
    #endif
    #ifdef AUDIO_STREAMING_EN
    DVR_CFG_DUMP_B(AUDIO_STREAMING_EN)
    #endif
    #ifdef AUI_STREAMING_EN
    DVR_CFG_DUMP_B(AUI_STREAMING_EN)
    #endif
    #ifdef MP12DEC_EN
    DVR_CFG_DUMP_B(MP12DEC_EN)
    #endif
    #ifdef GAPLESS_EN
    DVR_CFG_DUMP_B(GAPLESS_EN)
    #endif
    #ifdef HIGH_SRATE_MODE
    DVR_CFG_DUMP_B(HIGH_SRATE_MODE)
    #endif
    #ifdef AUDIO_CODEC_DUPLEX_EN
    DVR_CFG_DUMP_B(AUDIO_CODEC_DUPLEX_EN)
    #endif
    #ifdef I2S_CH_NUM
    DVR_CFG_DUMP_B(I2S_CH_NUM)
    #endif
    #ifdef DEFAULT_I2S_CH
    DVR_CFG_DUMP_B(DEFAULT_I2S_CH)
    #endif
}
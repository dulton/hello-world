//==============================================================================
//
//  File        : JobDispatch.c
//  Description : ce job dispatch function
//  Author      : Robin Feng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "Customer_Config.h"

#include "includes_fw.h"
#include "mmpf_fs_api.h"
#include "mmps_usb.h"
#include "mmps_vidplay.h"
#include "mmpf_sd.h"
#include "mmpf_usbvend.h"

#if (HDMI_OUTPUT_EN)
#include "mmp_reg_gbl.h"
#include "mmpf_i2cm.h"
#endif
#include "mmps_rtc.h"
#include "mmpf_wd.h"
#include "mmpf_saradc.h"
#include "mmpf_monitor.h"
#if (MEM_MONITOR_EN)
#include "mmps_dma.h"
#include "mmu.h"
#endif
#include "AHC_Common.h"
#include "fs_cfg.h"
#include "version.h"
#include "net_cfg.h"
#include "SD_Updater.h"
//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

MMP_BOOL					gbFileOpen = MMP_FALSE;
MMP_BYTE					gbEmergRecdFileName[20];

#if (EXT_CODEC_SUPPORT == 1) || (HDMI_OUTPUT_EN == 1)
static MMPS_AUDIO_I2S_CFG   m_I2Scfg;
#endif

char 						DefaultPlayDir[256] = {'S','D',':','\\'};
#if (HDMI_OUTPUT_EN)
static MMP_BOOL    			m_bHDMIPlugIn = MMP_FALSE;
static MMP_ULONG   			m_ulAudioPLL;
static MMP_ULONG   			m_ulAudioFreq;

const MMP_ULONG m_ulHDMIResolutionMap[8][2] =
{
	{640, 480},
	{720, 480},
	{720, 576},
	{1280,720},
	{1280,720},
	{1920,1080},
	{1920,1080},
	{1920,540}
};
#endif

static volatile MMP_BOOL    m_bAlarming = MMP_FALSE;

//==============================================================================
//
//                              EXTERN VARIABLE
//
//==============================================================================

extern MMP_ULONG glAhcBootComplete;         // CarDV
extern MMP_ULONG glVolume;
MMP_ULONG m_CurDisplayMode;
extern MMP_UBYTE gbEcoVer;

//==============================================================================
//
//                              FUNCTION PROTOTYPE
//
//==============================================================================

extern MMPS_USB_StatusMode MMPS_USB_GetStatus(void);
extern UINT8    AHC_GetBootingSrc(void);    // CarDV

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

void HexToDec(MMP_ULONG ulHex)
{
    MMP_USHORT  base, asciibase, j, i=0;
    MMP_BYTE    bResult[12], temp[12];

    asciibase = 48;     //ascii code '0' = 48;
    base = 10;
    
    do {
        temp[i]=(ulHex % base)+asciibase;
        ulHex = (ulHex-(ulHex % base))/base;
        i++;
    } while ((ulHex/base)>0);
    
    temp[i] = ulHex+asciibase;
    bResult[i+1] = '\0';

    //reverse the temp string and put it to the result
    for ( j=0; j<=i; j++){
        bResult[j]=temp[i-j];
    }
    
    RTNA_DBG_Str(0, bResult);
}

void HexToDecFixWidth(MMP_ULONG ulHex, MMP_LONG width)
{
    MMP_USHORT  base, asciibase, j, i = 0;
    MMP_BYTE    bResult[12], temp[12];

    asciibase = 48;     //ascii code '0' = 48;
    base = 10;
    
    do {
        temp[i]=(ulHex % base)+asciibase;
        ulHex = (ulHex - (ulHex % base)) / base;
        i++;
    } while(ulHex > 0);

    while(i < width) {
        temp[i++] = asciibase;
    }
    
    bResult[i] = '\0';
    i--;

    //reverse the temp string and put it to the result
    for ( j = 0; j <= i; j++) {
        bResult[j] = temp[i - j];
    }
    RTNA_DBG_Str(0, bResult);
}

MMP_ERR GetFileFromDirList(MMP_BYTE *dirname, MMP_BYTE *fullfilename)
{
    MMPS_FS_FILETIME 	FileTimeStru;
    MMP_ERR        		fsStatus;
    MMP_BYTE        	ret_file_name[100];
    #if (FS_INPUT_ENCODE == UCS2)
    MMP_BYTE        	UTF8_FName[50];
    #endif
    MMPS_FS_ATTRIBUTE 	attribute;
    MMP_ULONG       	EntrySize;
    MMP_UBYTE       	mode;
    MMP_ULONG       	open_dir_id, len;
    MMP_USHORT      	number;
    MMP_LONG        	i, j;
    MMP_BYTE        	str[20];
    MMP_BYTE        	filename[256];
    MMP_USHORT      	filenamelength;

    RTNA_DBG_Str(0, "\r\n-------------------------------------------\r\n");
    RTNA_DBG_Str(0, dirname);
    RTNA_DBG_Str(0, "\r\n");

    #if (FS_INPUT_ENCODE == UCS2)
    filenamelength = uniStrlen((short *)dirname);
    #elif (FS_INPUT_ENCODE == UTF8)
    filenamelength = STRLEN(dirname);
    #endif
    MEMCPY(filename, dirname, filenamelength);

    fsStatus = MMPS_FS_DirOpen (filename, filenamelength, &open_dir_id);

    if ( MMP_ERR_NONE != fsStatus ){
        RTNA_DBG_Str(0, "DirOpen failed\r\n");
        fsStatus = MMPS_FS_DirClose(open_dir_id);
        if (MMP_ERR_NONE != fsStatus){
            RTNA_DBG_Byte(0, fsStatus);
            RTNA_DBG_Str(0, "DirClose failed\r\n");
        }
        return fsStatus;
    }

    mode = MMPS_FILE_NAME | MMPS_ATTRIBUTE | MMPS_ENTRY_SIZE;
    i = 0;

    while(1) {

        fsStatus = MMPS_FS_GetFileList( open_dir_id,
                                        mode,
                                        ret_file_name,
                                        &attribute,
                                        &FileTimeStru,
                                        &EntrySize );

        if ( MMP_ERR_NONE != fsStatus ){
            if ( MMP_FS_ERR_NO_MORE_ENTRY == fsStatus ){
                break;
            }
            else{
                RTNA_DBG_Long(0, fsStatus);
                RTNA_DBG_Str(0, "GetFileList failed\r\n");
                break;
            }
        }
        
        HexToDec(i);
        
        if (MMPS_FS_ATTR_DIRECTORY & attribute) {
            RTNA_DBG_Str(0, " :         [");
            #if (FS_INPUT_ENCODE == UCS2)
            strfromUCS(UTF8_FName, ret_file_name);
            RTNA_DBG_Str(0, UTF8_FName);
            #else
            RTNA_DBG_Str(0, ret_file_name);
            #endif
            RTNA_DBG_Str(0, "] \r\n");
        }
        else {
            RTNA_DBG_Str(0, " : ");
            RTNA_DBG_Short(0, EntrySize/1048576);
            RTNA_DBG_Str(0, "MB ");
            #if (FS_INPUT_ENCODE == UCS2)
            strfromUCS(UTF8_FName, ret_file_name);
            RTNA_DBG_Str(0, UTF8_FName);
            #else
            RTNA_DBG_Str(0, ret_file_name);
            #endif
            RTNA_DBG_Str(0, "\r\n");
        }
        i++;
    }

    fsStatus = MMPS_FS_DirClose(open_dir_id);
    
    if ( MMP_ERR_NONE != fsStatus ){
        RTNA_DBG_Str(0, "DirClose failed.\r\n");
        return fsStatus;
    }

    RTNA_DBG_Str(0, "----------- File List ---------------------\r\n");
    RTNA_DBG_Str(0, " Num : Play\r\n");
    RTNA_DBG_Str(0, "Select file to operate:");
    str[0] = '\0';

    RTNA_DBG_Gets(str, &len);
    RTNA_DBG_Str(0, "\r\n");

    j = 0;
    number = 0;
    
    while(str[j]) {
        number = number * 10 + (str[j++] - '0');
    }
    
    if (number > (i-1)) {
        RTNA_DBG_Str(0, "Exceeded File index, Please enter valid file index to operate.\r\n");
        return (!MMP_ERR_NONE);
    }
    fsStatus = MMPS_FS_DirOpen(filename, filenamelength, &open_dir_id);

    for (i = 0; i <= number; i++) {
        fsStatus &= MMPS_FS_GetFileList(open_dir_id,
                        mode,
                        ret_file_name,
                        &attribute,
                        &FileTimeStru,
                        &EntrySize);
    }
    
    fsStatus &= MMPS_FS_DirClose(open_dir_id);
    
    if (fsStatus != MMP_ERR_NONE) {
        RTNA_DBG_Str(0, "Error on retriving file name in the list\r\n");
        return fsStatus;
    }

    if (!(MMPS_FS_ATTR_DIRECTORY & attribute) || (ret_file_name[0] != '.')) {
        #if (FS_INPUT_ENCODE == UCS2)
        uniStrcpy(fullfilename, dirname);
        uniStrcat(fullfilename, ret_file_name);
        #elif (FS_INPUT_ENCODE == UTF8)
        STRCPY(fullfilename, dirname);
        STRCAT(fullfilename, ret_file_name);
        #endif
    }
    else {
        #if (FS_INPUT_ENCODE == UCS2)
        strfromUCS(str, ret_file_name);
        strfromUCS(filename, dirname);
        filenamelength = uniStrlen(filenamelength);
        #elif (FS_INPUT_ENCODE == UTF8)
        STRCPY(str, ret_file_name);
        STRCPY(filename, dirname);
        filenamelength = STRLEN(filename);
        #endif
        
        if ((str[0] == '.') && (str[1] == '.')) {
            for (i = filenamelength - 2; i > 0; i--) {

                if (filename[i] == '\\')
                {
                    filename[i] = 0;
                    break;
                }
            }
        }
        else {
            for (i = filenamelength - 1; i > 0; i--) {

                if (filename[i] == '\\')
                {
                    filename[i] = 0;
                    break;
                }
            }
        }
        
        #if (FS_INPUT_ENCODE == UCS2)
        uniStrcpy(fullfilename, filename);
        #elif (FS_INPUT_ENCODE == UTF8)
        STRCPY(fullfilename, filename);
        #endif
    }

    if (MMPS_FS_ATTR_DIRECTORY & attribute) {
        #if (FS_INPUT_ENCODE == UCS2)
        strtoUCS(str, "\\");
        uniStrcat(fullfilename, str);
        uniStrcpy(DefaultPlayDir, fullfilename);
        #elif (FS_INPUT_ENCODE == UTF8)
        STRCAT(fullfilename, "\\");
        STRCPY(DefaultPlayDir, fullfilename);
        #endif
        GetFileFromDirList(fullfilename, fullfilename);
    }

    return MMP_ERR_NONE;

}

#if (HDMI_OUTPUT_EN)
void HDMI_HPD_TimerCallBack(OS_TMR *ptmr, void *p_arg)
{
    MMP_M_STATE     state;
    MMP_UBYTE       ubPllDiv = 0;
    #if 0
    MMP_I2CM_ATTR i2c_ddc = {
        MMP_I2CM_GPIO,    // i2cmID
        0x30,       	// ubSlaveAddr
        8,          	// ubRegLen
        8,          	// ubDataLen
        0x2,        	// ubDelayTime
        MMP_FALSE,  	// bDelayWaitEn
        MMP_FALSE, 	 	// bInputFilterEn
        MMP_FALSE,  	// b10BitModeEn
        MMP_FALSE,  	// bClkStretchEn
        0,          	// ubSlaveAddr1
        0,          	// ubDelayCycle
        0,          	// ubPadNum
        100000,     	// ulI2cmSpeed 100KHZ
        MMP_FALSE,  	// bOsProtectEn
        MMP_GPIO125,	// sw_clk_pin
        MMP_GPIO126,	// sw_data_pin
        MMP_FALSE,		// bRfclModeEn
        MMP_FALSE,  	// bWfclModeEn
        MMP_FALSE,      // bRepeatModeEn
        0               // ubVifPioMdlId
    };
    MMP_ULONG ulFormat, ulEDID;
    MMP_ULONG ulFormatMap = 0x1, ulShift = 0;
    #endif
    
    //test HDMI INT from CPU
    #if 0
    AITPS_AIC   pAIC = AITC_BASE_AIC;
    RTNA_AIC_Open(pAIC, AIC_SRC_HDMI_PHY, hdmi_isr_a,
	                AIC_INT_TO_IRQ | AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE | 3);
    RTNA_AIC_IRQ_En(pAIC, AIC_SRC_HDMI_PHY);
    pHDMI->HDMI_PHY_INT_CPU_EN = HDMI_PLUGIN_INT;
    #endif

    if(1) //HPD signal generation need HPD circuit support
    //pHDMI->HDMI_PHY_INT_CPU_SR & HDMI_PLUGIN_INT)
    {
        if(m_bHDMIPlugIn == MMP_FALSE) {

            MMPS_VIDPLAY_GetState(&state);
            if (MMP_M_STATE_EXECUTING == state) {
                MMPS_VIDPLAY_Pause();
            }

			#if 0
            MMPF_I2Cm_ReadEEDID(&i2c_ddc, m_ubEDIDData, 0, 0, 128);
            MMPD_HDMI_ParseEDID(m_ubEDIDData, &ulFormat, &ulEDID);

            MMPF_I2Cm_ReadEEDID(&i2c_ddc, m_ubEDIDData, 0, 128, 128);
            MMPD_HDMI_ParseEDID(m_ubEDIDData, &ulFormat, &ulEDID);
            if(ulEDID == 0xF0) {
                MMPF_I2Cm_ReadEEDID(&i2c_ddc, m_ubEDIDData, 1, 0, 128);
                MMPD_HDMI_ParseEDID(m_ubEDIDData, &ulFormat, &ulEDID);

                MMPF_I2Cm_ReadEEDID(&i2c_ddc, m_ubEDIDData, 1, 128, 128);
                MMPD_HDMI_ParseEDID(m_ubEDIDData, &ulFormat, &ulEDID);
            }
            VAR_L(0, ulFormat);

            if(ulFormat == 0) {
                DBG_S(0, "No supported HDMI output\r\n");
                return ;
            }

            while(ulFormatMap << ulShift) {
                MMP_ULONG ulVideo = (ulFormatMap << ulShift) & ulFormat;
                if(ulVideo) {
                    MMPS_Display_GetConfig()->hdmi.usDisplayWidth = m_ulHDMIResolutionMap[ulShift][0];
                    MMPS_Display_GetConfig()->hdmi.usDisplayHeight = m_ulHDMIResolutionMap[ulShift][1];
                    MMPS_Display_GetConfig()->hdmi.ubOutputMode = ulVideo;
                }
                ulShift++;
            }
			#endif
			
            MMPS_Display_GetConfig()->hdmi.usDisplayWidth =1920; 
            MMPS_Display_GetConfig()->hdmi.usDisplayHeight =1080;
            MMPS_Display_GetConfig()->hdmi.ubOutputMode = MMP_HDMI_OUTPUT_1920X1080P;

            MMPS_VIDPLAY_SetDisplayType(MMP_DISPLAY_SEL_HDMI);
            MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_HDMI);

            MMPS_Audio_GetConfig()->AudioOutI2SFormat = MMPS_AUDIO_I2S_MASTER;

            MMPS_System_GetGroupSrcAndDiv(CLK_GRP_AUD, &m_ulAudioPLL, &ubPllDiv);
            MMPS_System_GetGroupFreq(CLK_GRP_AUD, &m_ulAudioFreq);

            m_I2Scfg.workingMode      = MMPS_AUDIO_I2S_MASTER_MODE;
            m_I2Scfg.mclkMode         = MMPS_AUDIO_I2S_FIX_256FS_MODE;
            m_I2Scfg.outputBitSize    = MMPS_AUDIO_I2S_OUT_16BIT;
            m_I2Scfg.inputBitSize     = MMPS_AUDIO_I2S_IN_20BIT;
            m_I2Scfg.lrckPolarity     = MMPS_AUDIO_I2S_LRCK_L_LOW;
            m_I2Scfg.outDataDelayMode = MMPS_AUDIO_I2S_OUT_DELAY_STD;
            m_I2Scfg.bOutputBySDI     = MMP_FALSE;
            m_I2Scfg.bInputFromSDO    = MMP_FALSE;
            m_I2Scfg.ubInBitAlign     = 0;
            m_I2Scfg.ubOutBitAlign    = 16;

            MMPS_AUDIO_InitializeI2SConfig(DEFAULT_I2S_CH, &m_I2Scfg);
            MMPS_AUDIO_EnableI2SMclk(DEFAULT_I2S_CH, MMP_TRUE, 44100, 0);
            MMPS_AUDIO_SetOutPath(MMPS_AUDIO_OUT_I2S0);

            m_CurDisplayMode = VIDEOPLAY_MODE_HDMI;

            MMPS_VIDPLAY_SetDisplayMode(m_CurDisplayMode, MMP_DISPLAY_ROTATE_MAX, MMP_FALSE, NULL, NULL);

            MMPS_AUDIO_SetPlayVolumeDb(95);

            if (MMP_M_STATE_EXECUTING == state) {

                MMPS_VIDPLAY_Resume();
            }

            #if 0
            // Test OSD
            MMPD_Display_SetWinActive(MMP_DISPLAY_WIN_MAIN, MMP_FALSE);
            {
                MMP_ULONG addr = 0x2000000;
                MMP_ULONG i;
                MMP_DISPLAY_DISP_ATTR  dispattribute;
                MMP_DISPLAY_WIN_ATTR   winattribute;

                #define OSD_WIDTH 	320
                #define OSD_HEIGHT 	180

                for(i = 0; i < (OSD_WIDTH*OSD_HEIGHT*2); i+=2) {
                    if(i < OSD_WIDTH*(OSD_HEIGHT - 30)*2) {
                        *(MMP_USHORT *)(addr + i) = 0xF410;
                    } else {
                        *(MMP_USHORT *)(addr + i) = 0x001F;
                    }
                }

                addr = 0x2800000;

                for(i = 0; i < (OSD_WIDTH*OSD_HEIGHT*2); i+=2) {
                    if(i < OSD_WIDTH*(OSD_HEIGHT - 60)*2) {
                        *(MMP_USHORT *)(addr + i) = 0xF410;
                    } else {
                        *(MMP_USHORT *)(addr + i) = 0xF800;
                    }
                }

                MMPD_Display_SetWinPriority(MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_OVERLAY,
                                            MMP_DISPLAY_WIN_PIP, MMP_DISPLAY_WIN_ICON);
                                            
                winattribute.usWidth 			= OSD_WIDTH;
                winattribute.usHeight 			= OSD_HEIGHT;
                winattribute.usLineOffset 		= winattribute.usWidth * 2;
                winattribute.colordepth 		= MMP_DISPLAY_WIN_COLORDEPTH_16;
                winattribute.ulBaseAddr 		= 0x2000000;
                winattribute.ulBaseUAddr 		= 0x2000000;
                winattribute.ulBaseVAddr 		= 0x2000000;

                dispattribute.usStartX 			= 0;
                dispattribute.usStartY 			= 0;
                dispattribute.usDisplayWidth 	= winattribute.usWidth;
                dispattribute.usDisplayHeight 	= winattribute.usHeight;
                dispattribute.usDisplayOffsetX 	= 0;
                dispattribute.usDisplayOffsetY 	= 0;
                dispattribute.bMirror 			= 0;
                dispattribute.rotatetype 		= MMP_DISPLAY_ROTATE_NO_ROTATE;
                MMPD_Display_SetWinAttributes(MMP_DISPLAY_WIN_MAIN, &winattribute);
                MMPD_Display_SetWinToDisplay(MMP_DISPLAY_WIN_MAIN, &dispattribute);

                winattribute.ulBaseAddr 		= 0x2800000;
                winattribute.ulBaseUAddr 		= 0x2800000;
                winattribute.ulBaseVAddr 		= 0x2800000;
                MMPD_Display_SetWinAttributes(MMP_DISPLAY_WIN_OVERLAY, &winattribute);
                MMPD_Display_SetWinToDisplay(MMP_DISPLAY_WIN_OVERLAY, &dispattribute);

                // MAIN window
                MMPH_HIF_RegSetW(0x80007160, MMPH_HIF_RegGetW(0x80007160) | WIN_SRC_GRAB_EN);
                MMPH_HIF_RegSetW(0x80007162, (MMPH_HIF_RegGetW(0x80007162) & 0xC0) | (WIN_V_4X | WIN_H_4X));
                MMPH_HIF_RegSetW(0x80007150, MMPH_HIF_RegGetW(0x80007150) << 2);
                MMPH_HIF_RegSetW(0x80007154, MMPH_HIF_RegGetW(0x80007154) << 2);
                MMPH_HIF_RegSetL(0x80007168, MMPH_HIF_RegGetL(0x80007168) << 4);

                // OVERLAY window
                MMPH_HIF_RegSetW(0x80007360, MMPH_HIF_RegGetW(0x80007360) | WIN_SRC_GRAB_EN);
                MMPH_HIF_RegSetW(0x80007362, (MMPH_HIF_RegGetW(0x80007362) & 0xC0) | (WIN_V_4X | WIN_H_4X));
                MMPH_HIF_RegSetW(0x80007350, MMPH_HIF_RegGetW(0x80007350) << 2);
                MMPH_HIF_RegSetW(0x80007354, MMPH_HIF_RegGetW(0x80007354) << 2);
                MMPH_HIF_RegSetL(0x80007368, MMPH_HIF_RegGetL(0x80007368) << 4);

                MMPS_Display_SetWinTransparent(MMP_DISPLAY_WIN_MAIN, MMP_TRUE, 0xF410);
                MMPS_Display_SetWinTransparent(MMP_DISPLAY_WIN_OVERLAY, MMP_TRUE, 0xF410);

                MMPD_Display_SetWinActive(MMP_DISPLAY_WIN_MAIN, MMP_TRUE);
                MMPD_Display_SetWinActive(MMP_DISPLAY_WIN_OVERLAY, MMP_TRUE);
                MMPS_Display_SetDisplayRefresh(MMP_DISPLAY_PRM_CTL);
            }
            #endif
            m_bHDMIPlugIn = MMP_TRUE;
        }
    }
    else {
        if(m_bHDMIPlugIn == MMP_TRUE) {

            MMPS_VIDPLAY_GetState(&state);
            if (MMP_M_STATE_EXECUTING == state) {
                MMPS_VIDPLAY_Pause();
            }

            MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_MAIN_LCD);
            MMPS_VIDPLAY_SetDisplayType(MMP_DISPLAY_SEL_MAIN_LCD);
            MMPS_AUDIO_SetOutPath(MMPS_AUDIO_OUT_AFE_HP | MMPS_AUDIO_OUT_AFE_LINE);
            
            m_CurDisplayMode = VIDEOPLAY_MODE_STD_LCD;
            
            MMPS_VIDPLAY_SetDisplayMode(m_CurDisplayMode, MMP_DISPLAY_ROTATE_MAX, MMP_FALSE, NULL, NULL);

            /*{
                MMPF_GROUP_SRC Audio_PLL;
                MMP_UBYTE   ubClkDiv;
                MMP_ULONG ulAudioFreq = 0;
                MMPF_PLL_FREQ Audio_Freq;
                MMPF_PLL_GetGroupSrcAndDiv(MMPF_CLK_GRP_AUDIO, &Audio_PLL, &ubClkDiv);
                MMPF_PLL_GetGroupFreq(MMPF_CLK_GRP_AUDIO, &ulAudioFreq);
                if(ulAudioFreq == 48000) {
                    Audio_Freq = MMPF_PLL_FREQ_48MHz;
                } else if (ulAudioFreq == 96000) {
                    Audio_Freq = MMPF_PLL_FREQ_96MHz;
                } else if (ulAudioFreq == 144000) {
                    Audio_Freq = MMPF_PLL_FREQ_144MHz;
                } else if (ulAudioFreq == 192000) {
                    Audio_Freq = MMPF_PLL_FREQ_192MHz;
                } else {
                    DBG_S(0, "\r\nWrong Audio PLL Setting, PLease check!!\r\n\r\n");
                    Audio_Freq = MMPF_PLL_FREQ_192MHz;
                }
                MMPF_PLL_PowerUpWithFreq(Audio_PLL, Audio_Freq, 10);
                //pGBL->GBL_CLK_AUD_DIV = 0x04;
            }*/

            MMPS_AUDIO_SetPlayVolumeDb(95);

            if (MMP_M_STATE_EXECUTING == state) {

                MMPS_VIDPLAY_Resume();
            }
            m_bHDMIPlugIn = MMP_FALSE;
        }
    }
}
#endif


/*
 * Convert string (in Hex) to integer
 * Ex, "A1" to 161; "0E" to 14
 */
MMP_ULONG CharToInteger(char *Data, MMP_ULONG ulLen)
{
    MMP_ULONG ulVal = 0;
    char c;
    MMP_ULONG i = 0;
    
    while(ulLen !=0) {
        ulLen--;
        c = Data[ulLen];
        
        if(c >= '0' && c <= '9') {
            ulVal += ((c - '0') << (4*i));
        } else if (c >= 'A' && c <= 'F') {
            ulVal += ((c - 'A' + 10) << (4*i));
        } else if (c >= 'a' && c <= 'f') {
            ulVal += ((c - 'a' + 10) << (4*i));
        } else {
            DBG_S(0, "Charater to integer is fail, detect wrong charater\r\n\r\n");
        }
        i++;
    }
    return ulVal;
}

void RTC_AlarmCallback(void)
{
    RTNA_DBG_Str(0, "Alarm A On.\r\n");
    m_bAlarming = MMP_TRUE;
}

void RTC_SetAlarmAfter(MMP_UBYTE sec)
{
    AUTL_DATETIME datetime;

    MMPS_RTC_Initialize();

    datetime.usYear         = 2014;
    datetime.usMonth        = 9;
    datetime.usDay          = 3;
    datetime.usHour         = 1;
    datetime.usMinute       = 0;
    datetime.usSecond       = 0;
    datetime.ubAmOrPm       = 0;
    datetime.b_12FormatEn   = 0;
    datetime.usDayInWeek    = 1;
    MMPS_RTC_SetTime(&datetime);

    m_bAlarming     = MMP_FALSE;
    datetime.usMinute  += sec / 60;
    datetime.usSecond  += sec % 60;
    MMPS_RTC_SetWakeUpEn(MMP_TRUE, &datetime, RTC_AlarmCallback);

    while(!m_bAlarming) {
        MMPF_OS_Sleep(200);
    }
    MMPS_RTC_SetWakeUpEn(MMP_FALSE, NULL, NULL);
}

extern void UartCommandInit(void);
extern void UartCommandShell(void);

void CE_JOB_DISPATCH_Task(void *p_arg)
{
    MMP_USHORT ver_major, ver_minor, ver_patch;
    MMP_ULONG ulInitFs = 32000;
    extern AHC_BOOL		gAHC_InitialDone;    
    
    RTNA_DBG_Str0("AIT Job Task!\r\n");
    DVR_Version(&ver_major, &ver_minor, &ver_patch);
    RTNA_DBG_Str(0, "FirmwareCore Version:");
    RTNA_DBG_Short(0, ver_major);
    RTNA_DBG_Str(0, ".");
    RTNA_DBG_Short(0, ver_minor);
    RTNA_DBG_Str(0, ".");
    RTNA_DBG_Short(0, ver_patch);
    RTNA_DBG_Str(0, "\r\n");

#ifndef	NO_PANEL
		// InitOSD will make LCD off, Shutdown LCD backlight must before,
		// Otherwise, Some kind of LCD will show ugly white lines
		LedCtrl_LcdBackLight(AHC_FALSE);
		LedCtrl_LcdBackLightLock(AHC_TRUE);
#endif	// NO_PANEL

	InitOSD(); //Move to CE_JOB_DISPATCH_Task for fast boot.
    // Save the booting information ASAP
    AHC_GetBootingSrc();

    MMPS_System_AllocateFSBuffer(MMPS_SYSTEM_APMODE_DSC);
    glAhcBootComplete = MMP_TRUE;

    MMPS_AUDIO_PreDACPowerOn(ulInitFs); //Move to CE_JOB_DISPATCH_Task for fast boot.
    //InitOSD(); //Move to CE_JOB_DISPATCH_Task for fast boot.

    InitKeyPad(); //Move to CE_JOB_DISPATCH_Task for fast boot.
    AHC_DCF_Init();
	
	//Note : MAC addres burn in must follow AHC_DCF_Init()
	#if(WIFI_PORT && SD_MAC_POWERON_BURNIN)
	if(SDUpdateBackupMAC(SD_MAC_FILENAME))
		SDUpdateBurnInMAC();
	#endif
	
    UartCommandInit();

	gAHC_InitialDone = AHC_TRUE; //Start key/device detection after init done.
    UartCommandShell();
    //never run to here.
    while (1){
        MMPF_OS_Sleep(1000);
    }
}

/**
  @file ait_config.c
  @brief It contains the configurations need to be ported to the customer platform.
  @author Rogers

  @version
- 1.0 Original version
 */

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "ait_config.h"
#include "ait_utility.h"
#include "hdr_cfg.h"
#include "ptz_cfg.h"
#include "mmps_system.h"
#include "mmps_dsc.h"
#include "mmps_audio.h"
#include "mmps_display.h"
#include "mmps_vidplay.h"
#include "mmps_sensor.h"
#include "mmps_usb.h"
#include "mmps_3gprecd.h"
#include "lcd_common.h"
#include "AHC_Config_SDK.h"     // CarDV

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define QTABLE_NUM  (3)

//==============================================================================
//
//                              GLOBAL VARIABLE
//
//==============================================================================

static const MMP_UBYTE Q_table[QTABLE_NUM][128] = 
{
    // High quality
    0x08,0x06,0x06,0x06,0x06,0x06,0x08,0x08,
    0x08,0x08,0x0c,0x08,0x06,0x08,0x0c,0x0e,
    0x0a,0x08,0x08,0x0a,0x0e,0x10,0x0c,0x0c,
    0x0e,0x0c,0x0c,0x10,0x10,0x10,0x12,0x12,
    0x12,0x12,0x10,0x14,0x14,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,

    0x08,0x0a,0x0a,0x10,0x0e,0x10,0x14,0x14,
    0x14,0x14,0x20,0x14,0x14,0x14,0x20,0x20,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
    //Normal quality
    0x09,0x0a,0x0a,0x0a,0x0a,0x0a,0x0c,0x0c,
    0x0c,0x0c,0x10,0x0c,0x0a,0x0a,0x10,0x16,
    0x10,0x0c,0x0c,0x10,0x16,0x18,0x10,0x10,
    0x16,0x10,0x10,0x18,0x18,0x18,0x1c,0x1c,
    0x1c,0x1c,0x18,0x20,0x20,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,

    0x0a,0x10,0x10,0x18,0x16,0x18,0x20,0x20,
    0x20,0x20,0x28,0x20,0x20,0x20,0x28,0x28,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    //Low quality
    0x0b,0x0c,0x0c,0x0c,0x0c,0x0c,0x10,0x10,
    0x10,0x10,0x14,0x10,0x0c,0x0c,0x14,0x1a,
    0x14,0x10,0x10,0x14,0x1a,0x1c,0x14,0x14,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x24,0x24,0x24,0x24,0x24,0x24,0x24,0x24,
    0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,
    0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,
    0x28,0x28,0x28,0x28,0x28,0x28,0x28,0x28,

    0x0c,0x12,0x12,0x1a,0x1a,0x1c,0x24,0x24,
    0x24,0x24,0x2c,0x24,0x24,0x24,0x2c,0x2c,
    0x2c,0x2c,0x2c,0x2c,0x2c,0x2c,0x2c,0x2c,
    0x2c,0x2c,0x2c,0x2c,0x2c,0x2c,0x2c,0x2c,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
    0x30,0x30,0x30,0x30,0x30,0x30,0x30,0x30,
};

#if (AUDIO_SET_DB == 0x0)
static const MMP_UBYTE ubAudOutVol_HP_table[31][3] = 
{
    /* 0*-  mute*/ { 0,0x00,0x00}, /* 1*-49.5dB*/ {10,0x3F,0xAA}, /* 2*-48.0dB*/ {11,0x3F,0xAA}, /* 3*-46.5dB*/ {12,0x3F,0xAA}, /* 4*-45.0dB*/ {13,0x3F,0xAA},
    /* 5*-43.5dB*/ {14,0x3F,0xAA}, /* 6*-42.0dB*/ {15,0x3F,0xAA}, /* 7*-40.5dB*/ {16,0x3F,0xAA}, /* 8*-39.0dB*/ {17,0x3F,0xAA}, /* 9*-37.5dB*/ {18,0x3F,0xAA},
    /*10*-36.0dB*/ {19,0x3F,0xAA}, /*11*-34.5dB*/ {20,0x3F,0xAA}, /*12*-33.0dB*/ {21,0x3F,0xAA}, /*13*-31.5dB*/ {22,0x3F,0xAA}, /*14*-30.0dB*/ {23,0x3F,0xAA},
    /*15*-28.5dB*/ {24,0x3F,0xAA}, /*16*-27.0dB*/ {25,0x3F,0xAA}, /*17*-25.5dB*/ {26,0x3F,0xAA}, /*18*-24.0dB*/ {27,0x3F,0xAA}, /*19*-22.5dB*/ {28,0x3F,0xAA},
    /*20*-21.0dB*/ {29,0x3F,0xAA}, /*21*-19.5dB*/ {30,0x3F,0xAA}, /*22*-18.0dB*/ {31,0x3F,0xAA}, /*23*-16.5dB*/ {32,0x3F,0xAA}, /*24*-15.0dB*/ {33,0x3F,0xAA},
    /*25*-13.5dB*/ {34,0x3F,0xAA}, /*26*-12.0dB*/ {35,0x3F,0xAA}, /*27*-10.5dB*/ {36,0x3F,0xAA}, /*28*- 9  dB*/ {37,0x3F,0xAA}, /*29*- 7.5dB*/ {38,0x3F,0xAA},
    /*30*- 6  dB*/ {39,0x3F,0xAA},
};

static const MMP_UBYTE ubAudOutVol_Line_table[31][2] = 
{
    /* 0*-  Mute*/ { 0,0x00}, /* 1*-45.0dB*/ { 9,0x3F}, /* 2*-43.5dB*/ {10,0x3F}, /* 3*-42.0dB*/ {11,0x3F}, /* 4*-40.5dB*/ {12,0x3F},
    /* 5*-39.0dB*/ {13,0x3F}, /* 6*-37.5dB*/ {14,0x3F}, /* 7*-36.0dB*/ {15,0x3F}, /* 8*-34.5dB*/ {16,0x3F}, /* 9*-33.0dB*/ {17,0x3F},
    /*10*-31.5dB*/ {18,0x3F}, /*11*-30.0dB*/ {19,0x3F}, /*12*-28.5dB*/ {20,0x3F}, /*13*-27.0dB*/ {21,0x3F}, /*14*-25.5dB*/ {22,0x3F},
    /*15*-24.0dB*/ {23,0x3F}, /*16*-22.5dB*/ {24,0x3F}, /*17*-21.0dB*/ {25,0x3F}, /*18*-19.5dB*/ {26,0x3F}, /*19*-18.0dB*/ {27,0x3F},
    /*20*-16.5dB*/ {28,0x3F}, /*21*-15.0dB*/ {29,0x3F}, /*22*-13.5dB*/ {30,0x3F}, /*23*-12.0dB*/ {31,0x3F}, /*24*-10.5dB*/ {32,0x3F},
    /*25*- 9.0dB*/ {33,0x3F}, /*26*- 7.5dB*/ {34,0x3F}, /*27*- 6.0dB*/ {35,0x3F}, /*28*- 4.5dB*/ {36,0x3F}, /*29*- 3.0dB*/ {37,0x3F},
    /*30*- 1.5dB*/ {38,0x3F},
};
#else
static const MMP_UBYTE ubAudOutVol_HP_table[31][3] = 
{
    /* 0*-  mute*/ { 0,0x00,0x00}, /* 1*-49.5dB*/ {10,0x3F,0xAA}, /* 2*-48.0dB*/ {11,0x3F,0xAA}, /* 3*-46.5dB*/ {12,0x3F,0xAA}, /* 4*-45.0dB*/ {13,0x3F,0xAA},
    /* 5*-43.5dB*/ {14,0x3F,0xAA}, /* 6*-42.0dB*/ {15,0x3F,0xAA}, /* 7*-40.5dB*/ {16,0x3F,0xAA}, /* 8*-39.0dB*/ {17,0x3F,0xAA}, /* 9*-37.5dB*/ {18,0x3F,0xAA},
    /*10*-36.0dB*/ {19,0x3F,0xAA}, /*11*-34.5dB*/ {20,0x3F,0xAA}, /*12*-33.0dB*/ {21,0x3F,0xAA}, /*13*-31.5dB*/ {22,0x3F,0xAA}, /*14*-30.0dB*/ {23,0x3F,0xAA},
    /*15*-28.5dB*/ {24,0x3F,0xAA}, /*16*-27.0dB*/ {25,0x3F,0xAA}, /*17*-25.5dB*/ {26,0x3F,0xAA}, /*18*-24.0dB*/ {27,0x3F,0xAA}, /*19*-22.5dB*/ {28,0x3F,0xAA},
    /*20*-21.0dB*/ {29,0x3F,0xAA}, /*21*-19.5dB*/ {30,0x3F,0xAA}, /*22*-18.0dB*/ {31,0x3F,0xAA}, /*23*-16.5dB*/ {32,0x3F,0xAA}, /*24*-15.0dB*/ {33,0x3F,0xAA},
    /*25*-13.5dB*/ {34,0x3F,0xAA}, /*26*-12.0dB*/ {35,0x3F,0xAA}, /*27*-10.5dB*/ {36,0x3F,0xAA}, /*28*- 9  dB*/ {37,0x3F,0xAA}, /*29*- 7.5dB*/ {38,0x3F,0xAA},
    /*30*- 6  dB*/ {39,0x3F,0xAA},
};

static const MMP_UBYTE ubAudOutVol_Line_table[31][2] = 
{
    /* 0*-  Mute*/ { 0,0x00}, /* 1*-29.0dB*/ {67,0x3F}, /* 2*-28.0dB*/ {68,0x3F}, /* 3*-27.0dB*/ {69,0x3F}, /* 4*-26.0dB*/ {70,0x3F},
    /* 5*-25.0dB*/ {71,0x3F}, /* 6*-24.0dB*/ {72,0x3F}, /* 7*-23.0dB*/ {73,0x3F}, /* 8*-22.0dB*/ {74,0x3F}, /* 9*-21.0dB*/ {75,0x3F},
    /*10*-20.0dB*/ {76,0x3F}, /*11*-19.0dB*/ {77,0x3F}, /*12*-18.0dB*/ {78,0x3F}, /*13*-17.0dB*/ {79,0x3F}, /*14*-16.0dB*/ {80,0x3F},
    /*15*-15.0dB*/ {81,0x3F}, /*16*-14.0dB*/ {82,0x3F}, /*17*-13.0dB*/ {83,0x3F}, /*18*-12.0dB*/ {84,0x3F}, /*19*-11.0dB*/ {85,0x3F},
    /*20*-10.0dB*/ {86,0x3F}, /*21*- 9.0dB*/ {87,0x3F}, /*22*- 8.0dB*/ {88,0x3F}, /*23*- 7.0dB*/ {89,0x3F}, /*24*- 6.0dB*/ {90,0x3F},
    /*25*- 5.0dB*/ {91,0x3F}, /*26*- 4.0dB*/ {92,0x3F}, /*27*- 3.0dB*/ {93,0x3F}, /*28*- 2.0dB*/ {94,0x3F}, /*29*- 1.0dB*/ {95,0x3F},
    /*30*-   0dB*/ {96,0x3F}, 
};
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPC_Fdtc_InitConfig
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPC_Fdtc_InitConfig(void)
{
#if defined(ALL_FW)

    #if (SUPPORT_FDTC)
    MMPS_Fdtc_GetConfig()->faceDetectInputPath      = MMPS_IVA_FDTC_LOOPBACK; //one pipe for preview, one pipe for FDTC
    MMPS_Fdtc_GetConfig()->bFaceDetectEnable        = MMP_TRUE;
    MMPS_Fdtc_GetConfig()->bSmileDetectEnable       = MMP_FALSE;
    MMPS_Fdtc_GetConfig()->usFaceDetectGrabScaleM   = 80;
    MMPS_Fdtc_GetConfig()->ubDetectFaceNum          = 15; //Max: 15
    MMPS_Fdtc_GetConfig()->ubFaceDetectInputBufCnt  = 2;
    #endif
    
#endif
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_DSC_InitConfig
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPC_DSC_InitConfig(void)
{
#if !defined(MBOOT_FW)
    MMP_ULONG i, j;
    MMPS_DSC_CONFIG *pDscConfig = MMPS_DSC_GetConfig();

    /* DSC Preview Setting */
    pDscConfig->dispParams.ulRawPreviewBufCnt                           = 2;
    
    /* DSC Preview Mode 0 : DSC_NORMAL_PREVIEW_MODE */
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
    pDscConfig->dispParams.usDscPreviewBufW[DSC_NORMAL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_NORMAL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->dispParams.sFitMode[DSC_NORMAL_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;

    pDscConfig->dispParams.DispColorFmt[DSC_NORMAL_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV422;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    pDscConfig->dispParams.usDscPreviewBufW[DSC_NORMAL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_NORMAL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->dispParams.sFitMode[DSC_NORMAL_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;

    pDscConfig->dispParams.DispColorFmt[DSC_NORMAL_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_RGB565;
#endif

    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pDscConfig->dispParams.DispDevice[DSC_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pDscConfig->dispParams.DispDevice[DSC_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pDscConfig->dispParams.DispDevice[DSC_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pDscConfig->dispParams.DispDevice[DSC_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_NONE;
        break;
    }

    pDscConfig->dispParams.DispWinId[DSC_NORMAL_PREVIEW_MODE]           = MMP_DISPLAY_WIN_PIP;

    pDscConfig->dispParams.usDscDispBufCnt[DSC_NORMAL_PREVIEW_MODE]     = 2;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
    pDscConfig->dispParams.usDscDisplayW[DSC_NORMAL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->dispParams.usDscDisplayH[DSC_NORMAL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->dispParams.usDscDispStartX[DSC_NORMAL_PREVIEW_MODE]     = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_NORMAL_PREVIEW_MODE]     = (RTNA_LCD_GetAttr()->usPanelH - pDscConfig->dispParams.usDscDisplayH[DSC_NORMAL_PREVIEW_MODE]) / 2;
    pDscConfig->dispParams.bDscDispMirror[DSC_NORMAL_PREVIEW_MODE]      = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_NORMAL_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_NORMAL_PREVIEW_MODE]     = MMP_FALSE;
    
    pDscConfig->dispParams.bUseRotateDMA[DSC_NORMAL_PREVIEW_MODE]       = MMP_FALSE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_NORMAL_PREVIEW_MODE]      = 0;
    pDscConfig->dispParams.ubDMARotateDir[DSC_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    pDscConfig->dispParams.usDscDisplayW[DSC_NORMAL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->dispParams.usDscDisplayH[DSC_NORMAL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->dispParams.usDscDispStartX[DSC_NORMAL_PREVIEW_MODE]     = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_NORMAL_PREVIEW_MODE]     = (RTNA_LCD_GetAttr()->usPanelW - pDscConfig->dispParams.usDscDisplayW[DSC_NORMAL_PREVIEW_MODE]) / 2;
    pDscConfig->dispParams.bDscDispMirror[DSC_NORMAL_PREVIEW_MODE]      = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_NORMAL_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_NORMAL_PREVIEW_MODE]     = MMP_FALSE;
    
    pDscConfig->dispParams.bUseRotateDMA[DSC_NORMAL_PREVIEW_MODE]       = MMP_TRUE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_NORMAL_PREVIEW_MODE]      = 2;
    pDscConfig->dispParams.ubDMARotateDir[DSC_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_RIGHT_90;
#endif

    /* DSC Preview Mode 1 : DSC_FULL_PREVIEW_MODE */
    pDscConfig->dispParams.usDscPreviewBufW[DSC_FULL_PREVIEW_MODE]      = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_FULL_PREVIEW_MODE]      = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->dispParams.sFitMode[DSC_FULL_PREVIEW_MODE]              = MMP_SCAL_FITMODE_OUT;
    
    pDscConfig->dispParams.DispColorFmt[DSC_FULL_PREVIEW_MODE]          = MMP_DISPLAY_COLOR_RGB565;

    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pDscConfig->dispParams.DispDevice[DSC_FULL_PREVIEW_MODE]        = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pDscConfig->dispParams.DispDevice[DSC_FULL_PREVIEW_MODE]        = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pDscConfig->dispParams.DispDevice[DSC_FULL_PREVIEW_MODE]        = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pDscConfig->dispParams.DispDevice[DSC_FULL_PREVIEW_MODE]        = MMP_DISPLAY_NONE;
        break;
    }

    pDscConfig->dispParams.DispWinId[DSC_FULL_PREVIEW_MODE]             = MMP_DISPLAY_WIN_PIP;
    
    pDscConfig->dispParams.usDscDispBufCnt[DSC_FULL_PREVIEW_MODE]       = 2;
    pDscConfig->dispParams.usDscDisplayW[DSC_FULL_PREVIEW_MODE]         = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->dispParams.usDscDisplayH[DSC_FULL_PREVIEW_MODE]         = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->dispParams.usDscDispStartX[DSC_FULL_PREVIEW_MODE]       = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_FULL_PREVIEW_MODE]       = 0;
    pDscConfig->dispParams.bDscDispMirror[DSC_FULL_PREVIEW_MODE]        = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_FULL_PREVIEW_MODE]            = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_FULL_PREVIEW_MODE]       = MMP_FALSE;
    
    pDscConfig->dispParams.bUseRotateDMA[DSC_FULL_PREVIEW_MODE]         = MMP_TRUE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_FULL_PREVIEW_MODE]        = 2;
    pDscConfig->dispParams.ubDMARotateDir[DSC_FULL_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_RIGHT_90;

    /* DSC Preview Mode 2 : DSC_TV_NTSC_PREVIEW_MODE */
    pDscConfig->dispParams.usDscPreviewBufW[DSC_TV_NTSC_PREVIEW_MODE]   = 720;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_TV_NTSC_PREVIEW_MODE]   = 480;
    pDscConfig->dispParams.sFitMode[DSC_TV_NTSC_PREVIEW_MODE]           = MMP_SCAL_FITMODE_OUT;

    pDscConfig->dispParams.DispColorFmt[DSC_TV_NTSC_PREVIEW_MODE]       = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->dispParams.DispDevice[DSC_TV_NTSC_PREVIEW_MODE]         = MMP_DISPLAY_TV;
    pDscConfig->dispParams.DispWinId[DSC_TV_NTSC_PREVIEW_MODE]          = MMP_DISPLAY_WIN_PIP;

    pDscConfig->dispParams.usDscDispBufCnt[DSC_TV_NTSC_PREVIEW_MODE]    = 3;
    pDscConfig->dispParams.usDscDisplayW[DSC_TV_NTSC_PREVIEW_MODE]      = 720;
    pDscConfig->dispParams.usDscDisplayH[DSC_TV_NTSC_PREVIEW_MODE]      = 480;
    pDscConfig->dispParams.usDscDispStartX[DSC_TV_NTSC_PREVIEW_MODE]    = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_TV_NTSC_PREVIEW_MODE]    = 0;
    pDscConfig->dispParams.bDscDispMirror[DSC_TV_NTSC_PREVIEW_MODE]     = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_TV_NTSC_PREVIEW_MODE]         = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_TV_NTSC_PREVIEW_MODE]    = MMP_TRUE;
    
    pDscConfig->dispParams.bUseRotateDMA[DSC_TV_NTSC_PREVIEW_MODE]      = MMP_FALSE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_TV_NTSC_PREVIEW_MODE]     = 0;
    pDscConfig->dispParams.ubDMARotateDir[DSC_TV_NTSC_PREVIEW_MODE]     = MMP_DISPLAY_ROTATE_NO_ROTATE;

    /* DSC Preview Mode 3 : DSC_TV_PAL_PREVIEW_MODE */
    pDscConfig->dispParams.usDscPreviewBufW[DSC_TV_PAL_PREVIEW_MODE]    = 720;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_TV_PAL_PREVIEW_MODE]    = 576;
    pDscConfig->dispParams.sFitMode[DSC_TV_PAL_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;

    pDscConfig->dispParams.DispColorFmt[DSC_TV_PAL_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->dispParams.DispDevice[DSC_TV_PAL_PREVIEW_MODE]          = MMP_DISPLAY_TV;
    pDscConfig->dispParams.DispWinId[DSC_TV_PAL_PREVIEW_MODE]           = MMP_DISPLAY_WIN_PIP;

    pDscConfig->dispParams.usDscDispBufCnt[DSC_TV_PAL_PREVIEW_MODE]     = 3;
    pDscConfig->dispParams.usDscDisplayW[DSC_TV_PAL_PREVIEW_MODE]       = 720;
    pDscConfig->dispParams.usDscDisplayH[DSC_TV_PAL_PREVIEW_MODE]       = 576;
    pDscConfig->dispParams.usDscDispStartX[DSC_TV_PAL_PREVIEW_MODE]     = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_TV_PAL_PREVIEW_MODE]     = 0;
    pDscConfig->dispParams.bDscDispMirror[DSC_TV_PAL_PREVIEW_MODE]      = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_TV_PAL_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_TV_PAL_PREVIEW_MODE]     = MMP_TRUE;
    
    pDscConfig->dispParams.bUseRotateDMA[DSC_TV_PAL_PREVIEW_MODE]       = MMP_FALSE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_TV_PAL_PREVIEW_MODE]      = 0;
    pDscConfig->dispParams.ubDMARotateDir[DSC_TV_PAL_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;

    /* DSC Preview Mode 4 : DSC_HDMI_PREVIEW_MODE */
    pDscConfig->dispParams.usDscPreviewBufW[DSC_HDMI_PREVIEW_MODE]      = 1920;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_HDMI_PREVIEW_MODE]      = 1080;
    pDscConfig->dispParams.sFitMode[DSC_HDMI_PREVIEW_MODE]              = MMP_SCAL_FITMODE_OUT;

    pDscConfig->dispParams.DispColorFmt[DSC_HDMI_PREVIEW_MODE]          = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->dispParams.DispDevice[DSC_HDMI_PREVIEW_MODE]            = MMP_DISPLAY_HDMI;
    pDscConfig->dispParams.DispWinId[DSC_HDMI_PREVIEW_MODE]             = MMP_DISPLAY_WIN_PIP;

    pDscConfig->dispParams.usDscDispBufCnt[DSC_HDMI_PREVIEW_MODE]       = 2;
    pDscConfig->dispParams.usDscDisplayW[DSC_HDMI_PREVIEW_MODE]         = 1920;
    pDscConfig->dispParams.usDscDisplayH[DSC_HDMI_PREVIEW_MODE]         = 1080;
    pDscConfig->dispParams.usDscDispStartX[DSC_HDMI_PREVIEW_MODE]       = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_HDMI_PREVIEW_MODE]       = 0;
    pDscConfig->dispParams.bDscDispMirror[DSC_HDMI_PREVIEW_MODE]        = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_HDMI_PREVIEW_MODE]            = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_HDMI_PREVIEW_MODE]       = MMP_TRUE;
    
    pDscConfig->dispParams.bUseRotateDMA[DSC_HDMI_PREVIEW_MODE]         = MMP_FALSE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_HDMI_PREVIEW_MODE]        = 0;
    pDscConfig->dispParams.ubDMARotateDir[DSC_HDMI_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;

    /* DSC Preview Mode 5 : DSC_CCIR_PREVIEW_MODE */
    pDscConfig->dispParams.usDscPreviewBufW[DSC_CCIR_PREVIEW_MODE]      = 720;
    pDscConfig->dispParams.usDscPreviewBufH[DSC_CCIR_PREVIEW_MODE]      = 480;
    pDscConfig->dispParams.sFitMode[DSC_CCIR_PREVIEW_MODE]              = MMP_SCAL_FITMODE_OUT;

    pDscConfig->dispParams.DispColorFmt[DSC_CCIR_PREVIEW_MODE]          = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->dispParams.DispDevice[DSC_CCIR_PREVIEW_MODE]            = MMP_DISPLAY_CCIR;
    pDscConfig->dispParams.DispWinId[DSC_CCIR_PREVIEW_MODE]             = MMP_DISPLAY_WIN_PIP;

    pDscConfig->dispParams.usDscDispBufCnt[DSC_CCIR_PREVIEW_MODE]       = 3;
    pDscConfig->dispParams.usDscDisplayW[DSC_CCIR_PREVIEW_MODE]         = 720;
    pDscConfig->dispParams.usDscDisplayH[DSC_CCIR_PREVIEW_MODE]         = 480;
    pDscConfig->dispParams.usDscDispStartX[DSC_CCIR_PREVIEW_MODE]       = 0;
    pDscConfig->dispParams.usDscDispStartY[DSC_CCIR_PREVIEW_MODE]       = 0;
    pDscConfig->dispParams.bDscDispMirror[DSC_CCIR_PREVIEW_MODE]        = MMP_FALSE;
    pDscConfig->dispParams.DscDispDir[DSC_CCIR_PREVIEW_MODE]            = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->dispParams.bDscDispScaleEn[DSC_CCIR_PREVIEW_MODE]       = MMP_TRUE;

    pDscConfig->dispParams.bUseRotateDMA[DSC_CCIR_PREVIEW_MODE]         = MMP_FALSE;
    pDscConfig->dispParams.usRotateBufCnt[DSC_CCIR_PREVIEW_MODE]        = 0;
    pDscConfig->dispParams.ubDMARotateDir[DSC_CCIR_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;

    /* DSC Encode Mode parameter */
    pDscConfig->encParams.bStableShotEn                                 = MMP_FALSE;
    pDscConfig->encParams.usStableShotNum                               = 2;
    pDscConfig->encParams.bDscPreviewKeepOn                             = MMP_FALSE; //MMP_TRUE;

    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_3264x2448]           = 3264; //3264x2448 = 7990272
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_3264x2448]           = 2448;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_3200x2400]           = 3200; //3200x2400 = 7680000
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_3200x2400]           = 2400;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_3264x1836]           = 3264; //3264x1836 = 5992704
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_3264x1836]           = 1840;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_2816x2112]           = 2816; //2816x2112 = 5947392
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_2816x2112]           = 2112;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_2560x1920]           = 2560;
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_2560x1920]           = 1920; 
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_2048x1536]           = 2048;
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_2048x1536]           = 1536;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_2016x1512]           = 2016;
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_2016x1512]           = 1512;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_1600x1200]           = 1920;
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_1600x1200]           = 1080;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_1280x960]            = 1280;
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_1280x960]            = 960;
    pDscConfig->encParams.usDscEncW[DSC_JPEG_RESOL_640x480]             = 640;    
    pDscConfig->encParams.usDscEncH[DSC_JPEG_RESOL_640x480]             = 480;

    if (QTABLE_NUM > DSC_QTABLE_MAX_NUM) {
        PRINTF("ERROR: QTABLE_NUM > DSC_QTABLE_MAX_NUM\r\n");
    }
    else {
        for (i=0; i < QTABLE_NUM; i++) {
            for (j=0; j<128; j++) {
                pDscConfig->encParams.ubDscQtable[i][j] = Q_table[i][j];
            }
        }
    }
    
    pDscConfig->encParams.ulMaxThumbnailSize                        = 0x5000;       // 20KB
    #if (DSC_SUPPORT_BASELINE_MP_FILE)
    pDscConfig->encParams.ulMaxLargeThumbSize                       = 512 * 1024;   // 512KB
    #endif

    /* For JPEG decoder */
    pDscConfig->decParams.ulExifWorkingBufSize                      = 0x40000;  
    pDscConfig->decParams.ulDscDecMaxLineBufSize                    = 2560 * 32 * 3 + 1024; // 241KB
    pDscConfig->decParams.ulDscDecMaxCompBufSize                    = 6 * 1024 * 1024;      // 6MB
    pDscConfig->decParams.ulDscDecMaxInbufSize                      = 1 * 1024;             // 1KB

    // Decode to LCD normal mode
    pDscConfig->decParams.DecColorMode[DSC_NORMAL_DECODE_MODE]      = MMP_DISPLAY_COLOR_YUV420;
    pDscConfig->decParams.usDecodeOutW[DSC_NORMAL_DECODE_MODE]      = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->decParams.usDecodeOutH[DSC_NORMAL_DECODE_MODE]      = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->decParams.bMirror[DSC_NORMAL_DECODE_MODE]           = MMP_FALSE;
    pDscConfig->decParams.rotatetype[DSC_NORMAL_DECODE_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->decParams.fitmode[DSC_NORMAL_DECODE_MODE]           = MMP_SCAL_FITMODE_IN;
    pDscConfig->decParams.bDoubleOutputBuf[DSC_NORMAL_DECODE_MODE]  = MMP_FALSE;

    // Decode to LCD full rotate mode
    pDscConfig->decParams.DecColorMode[DSC_FULL_DECODE_MODE]        = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->decParams.usDecodeOutW[DSC_FULL_DECODE_MODE]        = RTNA_LCD_GetAttr()->usPanelH;
    pDscConfig->decParams.usDecodeOutH[DSC_FULL_DECODE_MODE]        = RTNA_LCD_GetAttr()->usPanelW;
    pDscConfig->decParams.bMirror[DSC_FULL_DECODE_MODE]             = MMP_FALSE;
    pDscConfig->decParams.rotatetype[DSC_FULL_DECODE_MODE]          = MMP_DISPLAY_ROTATE_RIGHT_90;
    pDscConfig->decParams.fitmode[DSC_FULL_DECODE_MODE]             = MMP_SCAL_FITMODE_OUT;
    pDscConfig->decParams.bDoubleOutputBuf[DSC_FULL_DECODE_MODE]    = MMP_FALSE;

    // Decode to TV NTSC mode
    pDscConfig->decParams.DecColorMode[DSC_TV_NTSC_DECODE_MODE]     = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->decParams.usDecodeOutW[DSC_TV_NTSC_DECODE_MODE]     = 720;
    pDscConfig->decParams.usDecodeOutH[DSC_TV_NTSC_DECODE_MODE]     = 480;
    pDscConfig->decParams.bMirror[DSC_TV_NTSC_DECODE_MODE]          = MMP_FALSE;
    pDscConfig->decParams.rotatetype[DSC_TV_NTSC_DECODE_MODE]       = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->decParams.fitmode[DSC_TV_NTSC_DECODE_MODE]          = MMP_SCAL_FITMODE_IN;
    pDscConfig->decParams.bDoubleOutputBuf[DSC_TV_NTSC_DECODE_MODE] = MMP_TRUE;

    // Decode to TV PAL mode
    pDscConfig->decParams.DecColorMode[DSC_TV_PAL_DECODE_MODE]      = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->decParams.usDecodeOutW[DSC_TV_PAL_DECODE_MODE]      = 720;
    pDscConfig->decParams.usDecodeOutH[DSC_TV_PAL_DECODE_MODE]      = 576;
    pDscConfig->decParams.bMirror[DSC_TV_PAL_DECODE_MODE]           = MMP_FALSE;
    pDscConfig->decParams.rotatetype[DSC_TV_PAL_DECODE_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->decParams.fitmode[DSC_TV_PAL_DECODE_MODE]           = MMP_SCAL_FITMODE_IN;
    pDscConfig->decParams.bDoubleOutputBuf[DSC_TV_PAL_DECODE_MODE]  = MMP_TRUE;

    // Decode to HDMI mode
    pDscConfig->decParams.DecColorMode[DSC_HDMI_DECODE_MODE]        = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->decParams.usDecodeOutW[DSC_HDMI_DECODE_MODE]        = 1920;
    pDscConfig->decParams.usDecodeOutH[DSC_HDMI_DECODE_MODE]        = 1080;
    pDscConfig->decParams.bMirror[DSC_HDMI_DECODE_MODE]             = MMP_FALSE;
    pDscConfig->decParams.rotatetype[DSC_HDMI_DECODE_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->decParams.fitmode[DSC_HDMI_DECODE_MODE]             = MMP_SCAL_FITMODE_IN;
    pDscConfig->decParams.bDoubleOutputBuf[DSC_HDMI_DECODE_MODE]    = MMP_TRUE;

    // Decode to CCIR mode
    pDscConfig->decParams.DecColorMode[DSC_CCIR_DECODE_MODE]        = MMP_DISPLAY_COLOR_YUV422;
    pDscConfig->decParams.usDecodeOutW[DSC_CCIR_DECODE_MODE]        = 720;
    pDscConfig->decParams.usDecodeOutH[DSC_CCIR_DECODE_MODE]        = 480;
    pDscConfig->decParams.bMirror[DSC_CCIR_DECODE_MODE]             = MMP_FALSE;
    pDscConfig->decParams.rotatetype[DSC_CCIR_DECODE_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pDscConfig->decParams.fitmode[DSC_CCIR_DECODE_MODE]             = MMP_SCAL_FITMODE_OUT;//MMP_SCAL_FITMODE_IN;
    pDscConfig->decParams.bDoubleOutputBuf[DSC_CCIR_DECODE_MODE]    = MMP_FALSE;
#endif
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_3gpRecd_InitConfig
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPC_3gpRecd_InitConfig(void)
{
#if !defined(MBOOT_FW)
    MMPS_3GPRECD_CONFIGS *pVideoConfig = MMPS_3GPRECD_GetConfig();

    // Video preview path selection
    #if (SUPPORT_LDC_RECD)
    pVideoConfig->previewpath[0] = MMP_3GP_PATH_LDC_LB_SINGLE;//MMP_3GP_PATH_LDC_LB_MULTI;
    #else
    pVideoConfig->previewpath[0] = MMP_3GP_PATH_2PIPE;
    #endif

    // Video preview setting : VIDRECD_NORMAL_PREVIEW_MODE
#if (SUPPORT_LDC_RECD)
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_NORMAL_PREVIEW_MODE]	= RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_NORMAL_PREVIEW_MODE]	= MMPF_LCD_GetBestRatioH(RTNA_LCD_GetAttr()->ubRatio, RTNA_LCD_GetAttr()->usPanelW);
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_NORMAL_PREVIEW_MODE]			= MMP_SCAL_FITMODE_OPTIMAL;	//MMP_SCAL_FITMODE_OUT;
#else    
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_NORMAL_PREVIEW_MODE]  = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_NORMAL_PREVIEW_MODE]  = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_NORMAL_PREVIEW_MODE]          = MMP_SCAL_FITMODE_OUT;
#endif

    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_NONE;
        break;
    }
    
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
    #if (SUPPORT_LDC_RECD)
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    #else
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420;
    #endif
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_NORMAL_PREVIEW_MODE]         = MMP_DISPLAY_WIN_PIP;
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]   = 2;
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_NORMAL_PREVIEW_MODE]   = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_NORMAL_PREVIEW_MODE]   = (RTNA_LCD_GetAttr()->usPanelH - pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_NORMAL_PREVIEW_MODE]) / 2;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_NORMAL_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_NORMAL_PREVIEW_MODE]     = MMP_FALSE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]    = 0;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_ROTATE_NO_ROTATE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    #if (SUPPORT_LDC_RECD)
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    #else
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420;
    #endif
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_NORMAL_PREVIEW_MODE]         = MMP_DISPLAY_WIN_PIP;
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]   = 2;
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_NORMAL_PREVIEW_MODE]   = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_NORMAL_PREVIEW_MODE]   = (RTNA_LCD_GetAttr()->usPanelH - pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]) / 2;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_NORMAL_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_NORMAL_PREVIEW_MODE]     = MMP_TRUE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]    = 2;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_ROTATE_RIGHT_90;
#endif

    // Video preview setting : VIDRECD_FULL_PREVIEW_MODE
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_FULL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_FULL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_FULL_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OPTIMAL;
    
    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pVideoConfig->previewdata[0].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_NONE;
        break;
    }
    
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_FULL_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_RGB565;
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_FULL_PREVIEW_MODE]           = MMP_DISPLAY_WIN_PIP;
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_FULL_PREVIEW_MODE]     = 2;
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_FULL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_FULL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_FULL_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_FULL_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_FULL_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_FULL_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;

    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_FULL_PREVIEW_MODE]       = MMP_TRUE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_FULL_PREVIEW_MODE]      = 2;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_RIGHT_90;
    
    // Video preview setting : VIDRECD_NTSC_PREVIEW_MODE
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_NTSC_PREVIEW_MODE]    = 720;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_NTSC_PREVIEW_MODE]    = 480;
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_NTSC_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;

    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_NTSC_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV422; //YUV is prefer input format for TV
    pVideoConfig->previewdata[0].DispDevice[VIDRECD_NTSC_PREVIEW_MODE]          = MMP_DISPLAY_TV;
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_NTSC_PREVIEW_MODE]           = MMP_DISPLAY_WIN_PIP;
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_NTSC_PREVIEW_MODE]     = 2;
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_NTSC_PREVIEW_MODE]       = 720;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_NTSC_PREVIEW_MODE]       = 480;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_NTSC_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_NTSC_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_NTSC_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_NTSC_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_NTSC_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_NTSC_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_NTSC_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;

    // Video preview setting : VIDRECD_PAL_PREVIEW_MODE
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_PAL_PREVIEW_MODE]     = 720;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_PAL_PREVIEW_MODE]     = 576;
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_PAL_PREVIEW_MODE]             = MMP_SCAL_FITMODE_OUT;
    
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_PAL_PREVIEW_MODE]         = MMP_DISPLAY_COLOR_YUV422; //YUV is prefer input format for TV
    pVideoConfig->previewdata[0].DispDevice[VIDRECD_PAL_PREVIEW_MODE]           = MMP_DISPLAY_TV;
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_PAL_PREVIEW_MODE]            = MMP_DISPLAY_WIN_PIP;
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_PAL_PREVIEW_MODE]      = 2;
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_PAL_PREVIEW_MODE]        = 720;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_PAL_PREVIEW_MODE]        = 576;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_PAL_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_PAL_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_PAL_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_PAL_PREVIEW_MODE]           = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_PAL_PREVIEW_MODE]        = MMP_FALSE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_PAL_PREVIEW_MODE]       = 0;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_PAL_PREVIEW_MODE]       = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    // Video preview setting : VIDRECD_HDMI_PREVIEW_MODE
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_HDMI_PREVIEW_MODE]    = 1920;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_HDMI_PREVIEW_MODE]    = 1080;
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_HDMI_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;
    
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_HDMI_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV420;
    pVideoConfig->previewdata[0].DispDevice[VIDRECD_HDMI_PREVIEW_MODE]          = MMP_DISPLAY_HDMI;
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_HDMI_PREVIEW_MODE]           = MMP_DISPLAY_WIN_PIP;
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_HDMI_PREVIEW_MODE]     = 3; // For HDMI preview have line issue .
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_HDMI_PREVIEW_MODE]       = 1920;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_HDMI_PREVIEW_MODE]       = 1080;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_HDMI_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_HDMI_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_HDMI_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_HDMI_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;    
    
    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_HDMI_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_HDMI_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_HDMI_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    // Video preview setting : VIDRECD_CCIR_PREVIEW_MODE
    pVideoConfig->previewdata[0].usVidPreviewBufW[VIDRECD_CCIR_PREVIEW_MODE]    = 720;
    pVideoConfig->previewdata[0].usVidPreviewBufH[VIDRECD_CCIR_PREVIEW_MODE]    = 480;
    pVideoConfig->previewdata[0].sFitMode[VIDRECD_CCIR_PREVIEW_MODE]			= MMP_SCAL_FITMODE_OPTIMAL;
    
    pVideoConfig->previewdata[0].DispColorFmt[VIDRECD_CCIR_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV422;
    pVideoConfig->previewdata[0].DispDevice[VIDRECD_CCIR_PREVIEW_MODE]          = MMP_DISPLAY_CCIR;
    pVideoConfig->previewdata[0].DispWinId[VIDRECD_CCIR_PREVIEW_MODE] 			= MMP_DISPLAY_WIN_PIP;    
    pVideoConfig->previewdata[0].usVidDispBufCnt[VIDRECD_CCIR_PREVIEW_MODE]     = 3; // For CCIR preview issue .
    pVideoConfig->previewdata[0].usVidDisplayW[VIDRECD_CCIR_PREVIEW_MODE]       = 720;
    pVideoConfig->previewdata[0].usVidDisplayH[VIDRECD_CCIR_PREVIEW_MODE]       = 480;
    pVideoConfig->previewdata[0].usVidDispStartX[VIDRECD_CCIR_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].usVidDispStartY[VIDRECD_CCIR_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[0].bVidDispMirror[VIDRECD_CCIR_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[0].VidDispDir[VIDRECD_CCIR_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;  
    
    pVideoConfig->previewdata[0].bUseRotateDMA[VIDRECD_CCIR_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[0].usRotateBufCnt[VIDRECD_CCIR_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[0].ubDMARotateDir[VIDRECD_CCIR_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;

#if (SUPPORT_DUAL_SNR)

    // Video preview path selection
    pVideoConfig->previewpath[1] = MMP_3GP_PATH_2PIPE;

    // Video preview setting : VIDRECD_NORMAL_PREVIEW_MODE
    pVideoConfig->previewdata[1].usVidPreviewBufW[VIDRECD_NORMAL_PREVIEW_MODE]  = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[1].usVidPreviewBufH[VIDRECD_NORMAL_PREVIEW_MODE]  = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[1].sFitMode[VIDRECD_NORMAL_PREVIEW_MODE]          = MMP_SCAL_FITMODE_OPTIMAL;
    
    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_NONE;
        break;
    }
    
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0)
    #if (SUPPORT_LDC_RECD)
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    #else
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV422;
    #endif
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_NORMAL_PREVIEW_MODE]         = MMP_DISPLAY_WIN_OVERLAY;
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]   = 2;
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_NORMAL_PREVIEW_MODE]   = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_NORMAL_PREVIEW_MODE]   = (RTNA_LCD_GetAttr()->usPanelH - pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]) / 2;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_NORMAL_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_NORMAL_PREVIEW_MODE]     = MMP_FALSE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]    = 0;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_ROTATE_NO_ROTATE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    #if (SUPPORT_LDC_RECD)
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420_INTERLEAVE;
    #else
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_NORMAL_PREVIEW_MODE]      = MMP_DISPLAY_COLOR_YUV420; //for IBC to do the following rotate procedure
    #endif
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_NORMAL_PREVIEW_MODE]         = MMP_DISPLAY_WIN_OVERLAY; // for rear cam
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]   = 2;
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]     = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_NORMAL_PREVIEW_MODE]   = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_NORMAL_PREVIEW_MODE]   = (RTNA_LCD_GetAttr()->usPanelH - pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_NORMAL_PREVIEW_MODE]) / 2;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_NORMAL_PREVIEW_MODE]        = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_NORMAL_PREVIEW_MODE]     = MMP_TRUE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_NORMAL_PREVIEW_MODE]    = 2;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_NORMAL_PREVIEW_MODE]    = MMP_DISPLAY_ROTATE_RIGHT_90;
#endif

    // Video preview setting : VIDRECD_FULL_PREVIEW_MODE
    pVideoConfig->previewdata[1].usVidPreviewBufW[VIDRECD_FULL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[1].usVidPreviewBufH[VIDRECD_FULL_PREVIEW_MODE]    = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[1].sFitMode[VIDRECD_FULL_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OPTIMAL;
    
    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pVideoConfig->previewdata[1].DispDevice[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_NONE;
        break;
    }
    
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_FULL_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_RGB565;
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_FULL_PREVIEW_MODE]           = MMP_DISPLAY_WIN_OVERLAY;
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_FULL_PREVIEW_MODE]     = 2;
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_FULL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelW;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_FULL_PREVIEW_MODE]       = RTNA_LCD_GetAttr()->usPanelH;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_FULL_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_FULL_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_FULL_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_FULL_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;

    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_FULL_PREVIEW_MODE]       = MMP_TRUE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_FULL_PREVIEW_MODE]      = 2;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_FULL_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_RIGHT_90;
    
    // Video preview setting : VIDRECD_NTSC_PREVIEW_MODE
    pVideoConfig->previewdata[1].usVidPreviewBufW[VIDRECD_NTSC_PREVIEW_MODE]    = 720;
    pVideoConfig->previewdata[1].usVidPreviewBufH[VIDRECD_NTSC_PREVIEW_MODE]    = 480;
    pVideoConfig->previewdata[1].sFitMode[VIDRECD_NTSC_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;

    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_NTSC_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV422; //YUV is prefer input format for TV
    pVideoConfig->previewdata[1].DispDevice[VIDRECD_NTSC_PREVIEW_MODE]          = MMP_DISPLAY_TV;
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_NTSC_PREVIEW_MODE]           = MMP_DISPLAY_WIN_OVERLAY;
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_NTSC_PREVIEW_MODE]     = 2;
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_NTSC_PREVIEW_MODE]       = 720;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_NTSC_PREVIEW_MODE]       = 480;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_NTSC_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_NTSC_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_NTSC_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_NTSC_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_NTSC_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_NTSC_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_NTSC_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;

    // Video preview setting : VIDRECD_PAL_PREVIEW_MODE
    pVideoConfig->previewdata[1].usVidPreviewBufW[VIDRECD_PAL_PREVIEW_MODE]     = 720;
    pVideoConfig->previewdata[1].usVidPreviewBufH[VIDRECD_PAL_PREVIEW_MODE]     = 576;
    pVideoConfig->previewdata[1].sFitMode[VIDRECD_PAL_PREVIEW_MODE]             = MMP_SCAL_FITMODE_OUT;
    
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_PAL_PREVIEW_MODE]         = MMP_DISPLAY_COLOR_YUV422; //YUV is prefer input format for TV
    pVideoConfig->previewdata[1].DispDevice[VIDRECD_PAL_PREVIEW_MODE]           = MMP_DISPLAY_TV;
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_PAL_PREVIEW_MODE]            = MMP_DISPLAY_WIN_OVERLAY;
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_PAL_PREVIEW_MODE]      = 2;
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_PAL_PREVIEW_MODE]        = 720;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_PAL_PREVIEW_MODE]        = 576;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_PAL_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_PAL_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_PAL_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_PAL_PREVIEW_MODE]           = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_PAL_PREVIEW_MODE]        = MMP_FALSE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_PAL_PREVIEW_MODE]       = 0;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_PAL_PREVIEW_MODE]       = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    // Video preview setting : VIDRECD_HDMI_PREVIEW_MODE
    pVideoConfig->previewdata[1].usVidPreviewBufW[VIDRECD_HDMI_PREVIEW_MODE]    = 1920;
    pVideoConfig->previewdata[1].usVidPreviewBufH[VIDRECD_HDMI_PREVIEW_MODE]    = 1080;
    pVideoConfig->previewdata[1].sFitMode[VIDRECD_HDMI_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;
    
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_HDMI_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV420;
    pVideoConfig->previewdata[1].DispDevice[VIDRECD_HDMI_PREVIEW_MODE]          = MMP_DISPLAY_HDMI;
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_HDMI_PREVIEW_MODE]           = MMP_DISPLAY_WIN_OVERLAY;
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_HDMI_PREVIEW_MODE]     = 3; // For HDMI preview have line issue .
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_HDMI_PREVIEW_MODE]       = 1920;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_HDMI_PREVIEW_MODE]       = 1080;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_HDMI_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_HDMI_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_HDMI_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_HDMI_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;    
    
    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_HDMI_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_HDMI_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_HDMI_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;
    
    // Video preview setting : VIDRECD_CCIR_PREVIEW_MODE
    pVideoConfig->previewdata[1].usVidPreviewBufW[VIDRECD_CCIR_PREVIEW_MODE]    = 720;
    pVideoConfig->previewdata[1].usVidPreviewBufH[VIDRECD_CCIR_PREVIEW_MODE]    = 480;
    pVideoConfig->previewdata[1].sFitMode[VIDRECD_CCIR_PREVIEW_MODE]            = MMP_SCAL_FITMODE_OUT;
    
    pVideoConfig->previewdata[1].DispColorFmt[VIDRECD_CCIR_PREVIEW_MODE]        = MMP_DISPLAY_COLOR_YUV422;
    pVideoConfig->previewdata[1].DispDevice[VIDRECD_CCIR_PREVIEW_MODE]          = MMP_DISPLAY_CCIR;
    pVideoConfig->previewdata[1].DispWinId[VIDRECD_CCIR_PREVIEW_MODE] 			= MMP_DISPLAY_WIN_PIP;
    
    pVideoConfig->previewdata[1].usVidDispBufCnt[VIDRECD_CCIR_PREVIEW_MODE]     = 3; // For CCIR preview issue .
    pVideoConfig->previewdata[1].usVidDisplayW[VIDRECD_CCIR_PREVIEW_MODE]       = 720;
    pVideoConfig->previewdata[1].usVidDisplayH[VIDRECD_CCIR_PREVIEW_MODE]       = 480;
    pVideoConfig->previewdata[1].usVidDispStartX[VIDRECD_CCIR_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].usVidDispStartY[VIDRECD_CCIR_PREVIEW_MODE]     = 0;
    pVideoConfig->previewdata[1].bVidDispMirror[VIDRECD_CCIR_PREVIEW_MODE]      = MMP_FALSE;
    pVideoConfig->previewdata[1].VidDispDir[VIDRECD_CCIR_PREVIEW_MODE]          = MMP_DISPLAY_ROTATE_NO_ROTATE;  
    
    pVideoConfig->previewdata[1].bUseRotateDMA[VIDRECD_CCIR_PREVIEW_MODE]       = MMP_FALSE;
    pVideoConfig->previewdata[1].usRotateBufCnt[VIDRECD_CCIR_PREVIEW_MODE]      = 0;
    pVideoConfig->previewdata[1].ubDMARotateDir[VIDRECD_CCIR_PREVIEW_MODE]      = MMP_DISPLAY_ROTATE_NO_ROTATE;

#endif // SUPPORT_DUAL_SNR

    // Video encode setting : Encode Width / Height
    pVideoConfig->usEncWidth[VIDRECD_RESOL_160x120]         = 160;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_160x120]        = 120; 
    pVideoConfig->usEncWidth[VIDRECD_RESOL_176x144]         = 176;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_176x144]        = 144;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_272x144]         = 272;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_272x144]        = 144;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_320x240]         = 320;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_320x240]        = 240;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_352x288]         = 352;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_352x288]        = 288;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_368x208]         = 368;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_368x208]        = 208;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_480x272]         = 480;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_480x272]        = 272;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_624x352]         = 624;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_624x352]        = 352;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_640x368]         = 640;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_640x368]        = 368;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_640x480]         = 640;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_640x480]        = 480;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_720x480]         = 720;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_720x480]        = 480;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_848x480]         = 840;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_848x480]        = 480;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_912x512]         = 912;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_912x512]        = 512;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_960x720]         = 960;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_960x720]        = 720;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_1280x720]        = 1280;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_1280x720]       = 720;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_1440x1088]       = 1440;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_1440x1088]      = 1088;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_1920x1088]       = 1920;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_1920x1088]      = 1088;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_2304x1296]       = 2304;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_2304x1296]      = 1296;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_2704x1536]       = 2704;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_2704x1536]      = 1536;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_3840x2160]       = 3840;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_3840x2160]      = 2160;
    pVideoConfig->usEncWidth[VIDRECD_RESOL_2560x1440]       = 2560;
    pVideoConfig->usEncHeight[VIDRECD_RESOL_2560x1440]      = 1440;

    // Video encode setting : Bitrate
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_160x120][VIDRECD_QUALITY_LOW]     = 160000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_160x120][VIDRECD_QUALITY_MID]     = 240000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_160x120][VIDRECD_QUALITY_HIGH]    = 320000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_160x120][VIDRECD_QUALITY_LOW]     = 320000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_160x120][VIDRECD_QUALITY_MID]     = 480000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_160x120][VIDRECD_QUALITY_HIGH]    = 640000;
    
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_176x144][VIDRECD_QUALITY_LOW]     = 160000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_176x144][VIDRECD_QUALITY_MID]     = 240000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_176x144][VIDRECD_QUALITY_HIGH]    = 320000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_176x144][VIDRECD_QUALITY_LOW]     = 320000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_176x144][VIDRECD_QUALITY_MID]     = 480000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_176x144][VIDRECD_QUALITY_HIGH]    = 640000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_272x144][VIDRECD_QUALITY_LOW]     = 160000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_272x144][VIDRECD_QUALITY_MID]     = 240000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_272x144][VIDRECD_QUALITY_HIGH]    = 320000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_272x144][VIDRECD_QUALITY_LOW]     = 320000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_272x144][VIDRECD_QUALITY_MID]     = 480000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_272x144][VIDRECD_QUALITY_HIGH]    = 640000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_320x240][VIDRECD_QUALITY_LOW]     = 480000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_320x240][VIDRECD_QUALITY_MID]     = 720000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_320x240][VIDRECD_QUALITY_HIGH]    = 960000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_320x240][VIDRECD_QUALITY_LOW]     = 960000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_320x240][VIDRECD_QUALITY_MID]     = 1480000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_320x240][VIDRECD_QUALITY_HIGH]    = 1920000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_352x288][VIDRECD_QUALITY_LOW]     = 640000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_352x288][VIDRECD_QUALITY_MID]     = 960000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_352x288][VIDRECD_QUALITY_HIGH]    = 1280000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_352x288][VIDRECD_QUALITY_LOW]     = 1280000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_352x288][VIDRECD_QUALITY_MID]     = 1920000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_352x288][VIDRECD_QUALITY_HIGH]    = 2560000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_368x208][VIDRECD_QUALITY_LOW]     = 640000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_368x208][VIDRECD_QUALITY_MID]     = 960000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_368x208][VIDRECD_QUALITY_HIGH]    = 1280000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_368x208][VIDRECD_QUALITY_LOW]     = 1280000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_368x208][VIDRECD_QUALITY_MID]     = 1920000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_368x208][VIDRECD_QUALITY_HIGH]    = 2560000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_480x272][VIDRECD_QUALITY_LOW]     = 640000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_480x272][VIDRECD_QUALITY_MID]     = 960000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_480x272][VIDRECD_QUALITY_HIGH]    = 1280000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_480x272][VIDRECD_QUALITY_LOW]     = 1280000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_480x272][VIDRECD_QUALITY_MID]     = 1920000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_480x272][VIDRECD_QUALITY_HIGH]    = 2560000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_624x352][VIDRECD_QUALITY_LOW]     = 1920000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_624x352][VIDRECD_QUALITY_MID]     = 2880000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_624x352][VIDRECD_QUALITY_HIGH]    = 3840000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_624x352][VIDRECD_QUALITY_LOW]     = 3840000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_624x352][VIDRECD_QUALITY_MID]     = 5760000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_624x352][VIDRECD_QUALITY_HIGH]    = 7680000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_640x368][VIDRECD_QUALITY_LOW]     = 1920000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_640x368][VIDRECD_QUALITY_MID]     = 2880000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_640x368][VIDRECD_QUALITY_HIGH]    = 3840000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_640x368][VIDRECD_QUALITY_LOW]     = 3840000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_640x368][VIDRECD_QUALITY_MID]     = 5760000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_640x368][VIDRECD_QUALITY_HIGH]    = 7680000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_640x480][VIDRECD_QUALITY_LOW]     = 1920000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_640x480][VIDRECD_QUALITY_MID]     = AHC_VIDEO_BITRATE_VGA30FPS_FINE;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_640x480][VIDRECD_QUALITY_HIGH]    = AHC_VIDEO_BITRATE_VGA30FPS_SUPPERFINE;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_640x480][VIDRECD_QUALITY_LOW]     = 3840000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_640x480][VIDRECD_QUALITY_MID]     = 5760000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_640x480][VIDRECD_QUALITY_HIGH]    = 7680000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_720x480][VIDRECD_QUALITY_LOW]     = 2000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_720x480][VIDRECD_QUALITY_MID]     = 3000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_720x480][VIDRECD_QUALITY_HIGH]    = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_720x480][VIDRECD_QUALITY_LOW]     = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_720x480][VIDRECD_QUALITY_MID]     = 6000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_720x480][VIDRECD_QUALITY_HIGH]    = 8000000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_848x480][VIDRECD_QUALITY_LOW]     = 2000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_848x480][VIDRECD_QUALITY_MID]     = 3000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_848x480][VIDRECD_QUALITY_HIGH]    = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_848x480][VIDRECD_QUALITY_LOW]     = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_848x480][VIDRECD_QUALITY_MID]     = 6000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_848x480][VIDRECD_QUALITY_HIGH]    = 8000000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_912x512][VIDRECD_QUALITY_LOW]     = 2000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_912x512][VIDRECD_QUALITY_MID]     = 3000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_912x512][VIDRECD_QUALITY_HIGH]    = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_912x512][VIDRECD_QUALITY_LOW]     = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_912x512][VIDRECD_QUALITY_MID]     = 6000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_912x512][VIDRECD_QUALITY_HIGH]    = 8000000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_960x720][VIDRECD_QUALITY_LOW]     = 2000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_960x720][VIDRECD_QUALITY_MID]     = 3000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_960x720][VIDRECD_QUALITY_HIGH]    = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_960x720][VIDRECD_QUALITY_LOW]     = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_960x720][VIDRECD_QUALITY_MID]     = 6000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_960x720][VIDRECD_QUALITY_HIGH]    = 8000000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_LOW]    = 2000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_MID]    = AHC_VIDEO_BITRATE_HD30FPS_FINE;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_HIGH]   = AHC_VIDEO_BITRATE_HD30FPS_SUPPERFINE; 
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_LOW]    = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_MID]    = AHC_VIDEO_BITRATE_HD60FPS_FINE;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1280x720][VIDRECD_QUALITY_HIGH]   = AHC_VIDEO_BITRATE_HD60FPS_SUPPERFINE; 

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1440x1088][VIDRECD_QUALITY_LOW]  = 4000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1440x1088][VIDRECD_QUALITY_MID]  = 11440000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1440x1088][VIDRECD_QUALITY_HIGH] = 22880000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1440x1088][VIDRECD_QUALITY_LOW]  = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1440x1088][VIDRECD_QUALITY_MID]  = 11440000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1440x1088][VIDRECD_QUALITY_HIGH] = 22880000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_LOW]   = 4000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_MID]   = AHC_VIDEO_BITRATE_FHD30FPS_FINE;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_HIGH]  = AHC_VIDEO_BITRATE_FHD30FPS_SUPPERFINE;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_LOW]   = 8000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_MID]   = AHC_VIDEO_BITRATE_FHD60FPS_FINE;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_1920x1088][VIDRECD_QUALITY_HIGH]  = AHC_VIDEO_BITRATE_FHD60FPS_SUPPERFINE;
    
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2304x1296][VIDRECD_QUALITY_LOW]   = 5720000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2304x1296][VIDRECD_QUALITY_MID]   = AHC_VIDEO_BITRATE_SHD30FPS_FINE;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2304x1296][VIDRECD_QUALITY_HIGH]  = AHC_VIDEO_BITRATE_SHD30FPS_SUPPERFINE;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2304x1296][VIDRECD_QUALITY_LOW]   = 5720000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2304x1296][VIDRECD_QUALITY_MID]   = 11440000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2304x1296][VIDRECD_QUALITY_HIGH]  = 22880000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2560x1440][VIDRECD_QUALITY_LOW]   = 5720000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2560x1440][VIDRECD_QUALITY_MID]   = AHC_VIDEO_BITRATE_SHD30FPS_FINE;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2560x1440][VIDRECD_QUALITY_HIGH]  = AHC_VIDEO_BITRATE_SHD30FPS_SUPPERFINE;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2560x1440][VIDRECD_QUALITY_LOW]   = 5720000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2560x1440][VIDRECD_QUALITY_MID]   = 11440000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2560x1440][VIDRECD_QUALITY_HIGH]  = 22880000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2704x1536][VIDRECD_QUALITY_LOW]   = 4000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2704x1536][VIDRECD_QUALITY_MID]   = 8000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_2704x1536][VIDRECD_QUALITY_HIGH]  = 16000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2704x1536][VIDRECD_QUALITY_LOW]   = 8000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2704x1536][VIDRECD_QUALITY_MID]   = 16000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_2704x1536][VIDRECD_QUALITY_HIGH]  = 32000000;

    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_3840x2160][VIDRECD_QUALITY_LOW]   = 4000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_3840x2160][VIDRECD_QUALITY_MID]   = 8000000;
    pVideoConfig->ulFps30BitrateMap[VIDRECD_RESOL_3840x2160][VIDRECD_QUALITY_HIGH]  = 16000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_3840x2160][VIDRECD_QUALITY_LOW]   = 4000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_3840x2160][VIDRECD_QUALITY_MID]   = 8000000;
    pVideoConfig->ulFps60BitrateMap[VIDRECD_RESOL_3840x2160][VIDRECD_QUALITY_HIGH]  = 16000000;
    
    // Video encode setting : Frame Rate
    pVideoConfig->framerate[VIDRECD_FRAMERATE_7d5FPS].usVopTimeIncrement    = 8000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_7d5FPS].usVopTimeIncrResol    = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_10FPS].usVopTimeIncrement     = 6000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_10FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_12FPS].usVopTimeIncrement     = 5000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_12FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_12d5FPS].usVopTimeIncrement   = 4800;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_12d5FPS].usVopTimeIncrResol   = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_15FPS].usVopTimeIncrement     = 4000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_15FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_20FPS].usVopTimeIncrement     = 3000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_20FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_24FPS].usVopTimeIncrement     = 2407;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_24FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_25FPS].usVopTimeIncrement     = 2400;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_25FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_30FPS].usVopTimeIncrement     = 2000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_30FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_50FPS].usVopTimeIncrement     = 1200;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_50FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_60FPS].usVopTimeIncrement     = 1000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_60FPS].usVopTimeIncrResol     = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_100FPS].usVopTimeIncrement    = 600;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_100FPS].usVopTimeIncrResol    = 60000;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_120FPS].usVopTimeIncrement    = 500;
    pVideoConfig->framerate[VIDRECD_FRAMERATE_120FPS].usVopTimeIncrResol    = 60000;

    // Feature setting
    pVideoConfig->bAsyncMode                = MMP_FALSE;
    pVideoConfig->bSeamlessMode             = MMP_TRUE;
    pVideoConfig->bFdtcEnable               = MMP_FALSE;
    if (gsHdrCfg.bEnable || gsVidPtzCfg.bEnable)
        pVideoConfig->bRawPreviewEnable[0]  = MMP_TRUE;
    else
        pVideoConfig->bRawPreviewEnable[0]  = MMP_FALSE;
    #if (SUPPORT_EIS)
    pVideoConfig->bRawPreviewEnable[0]      = MMP_TRUE;
    #endif
    pVideoConfig->ubRawPreviewBitMode[0]    = MMP_RAW_COLORFMT_BAYER8;
    #if (SUPPORT_DUAL_SNR)
    pVideoConfig->bRawPreviewEnable[1]      = MMP_TRUE;
    #if (SCD_SNR_STORE_YUV420)
    pVideoConfig->ubRawPreviewBitMode[1]    = MMP_RAW_COLORFMT_YUV420;
    #else
    pVideoConfig->ubRawPreviewBitMode[1]    = MMP_RAW_COLORFMT_YUV422;
    #endif
    #endif
    pVideoConfig->bStillCaptureEnable       = MMP_TRUE;
    #if (HANDLE_JPEG_EVENT_BY_QUEUE) || (SUPPORT_DUAL_SNR)
    pVideoConfig->bDualCaptureEnable        = MMP_TRUE;
    #else
    pVideoConfig->bDualCaptureEnable        = MMP_FALSE;
    #endif
    #if (SUPPORT_VR_WIFI_STREAM)
    pVideoConfig->bWifiStreamEnable         = MMP_TRUE;
    pVideoConfig->ulMaxWifiStreamWidth		= 640;
    pVideoConfig->ulMaxWifiStreamHeight		= 480;
    #else
    pVideoConfig->bWifiStreamEnable         = MMP_FALSE;
    #endif
    
    // Buffer Reserved Size 
    pVideoConfig->ulRawStoreBufCnt          = 2;

    // Video: 8(bytes / frame), Audio 4: (bytes / frame)
    // Video: 8(bytes / frame) * 30 (frames / sec) = 240 (bytes / sec)
    // Audio: 48k(sample / sec) * 4 (bytes / frame) /1024(sample / frame) = 187.5 (bytes /sec)
    // Total 25 mins: (240 + 187.5) * 60 * 25 = 641,250 (bytes) = 0x9C8E2
    pVideoConfig->ulTailBufSize             = 0xA0000;

    #if (SHARE_REF_GEN_BUF == 1)
#ifdef MCR_V2_32MB
    pVideoConfig->ulVideoCompBufSize        = 3584*1024; //8*1024*1024;
#else
    pVideoConfig->ulVideoCompBufSize        = 8*1024*1024;
#endif
    #else
        #if (EMERGENTRECD_SUPPORT == 1)
        pVideoConfig->ulVideoCompBufSize    = 20*1024*1024;
        #else
        pVideoConfig->ulVideoCompBufSize    = 8*1024*1024;
        #if (DUALENC_SUPPORT == 1)
        pVideoConfig->ulVideoCompBufSize    = 12*1024*1024;
        #endif
        #endif
    #endif

    #if (EMERGENTRECD_SUPPORT == 1)
    pVideoConfig->ulAudioCompBufSize        = 1024*1024;
    #else
#ifdef MCR_V2_32MB
    pVideoConfig->ulAudioCompBufSize        = 300*1024;
#else
    pVideoConfig->ulAudioCompBufSize        = 750*1024;
#endif
    #endif

    #if (UVC_VIDRECD_SUPPORT == 1)
    pVideoConfig->ulUVCVidCompBufSize       = pVideoConfig->ulVideoCompBufSize;     //follow ulVideoCompBufSize to achieve best performance
    #if (VIDRECD_MULTI_TRACK == 0)
    // CarDV... TBD.  It relates to pre-encode + dual cam video record.
    if (pVideoConfig->ulTailBufSize)
        pVideoConfig->ulUVCTailBufSize      = pVideoConfig->ulTailBufSize;
    else
        pVideoConfig->ulUVCTailBufSize      = 0xA0000;
    #endif
    #endif

    #if (DUALENC_SUPPORT == 1)
    #ifdef MCR_V2_32MB
    pVideoConfig->ul2ndStreamVidCompBufSize = 1*1024*1024; //For TV decoder (480P), only set 1MB.
    #else
    pVideoConfig->ul2ndStreamVidCompBufSize = pVideoConfig->ulVideoCompBufSize;
    #endif
    #endif

    #if (UVC_EMERGRECD_SUPPORT == 1 && VIDRECD_MULTI_TRACK == 0)
    pVideoConfig->ulUVCEmergTailBufSize     = 0x90000;
    #endif

    #if (VIDRECD_REFIXRECD_SUPPORT == 1)
    pVideoConfig->ulReFixTailBufSize        = 0xA0000;
    #endif

#endif

    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_3gpPlay_InitConfig
//  Description :
//------------------------------------------------------------------------------
/** @brief Initialized the preset configurations for 3GP playing

@pre This function must be called after MMPC_Display_InitConfig because of the TV attribute dependence
@pre The DRAM is initlized so that the DRAM type and size are assigned.
@return Always returns true;
*/
MMP_BOOL MMPC_3gpPlay_InitConfig(void)
{
#if !defined(MBOOT_FW)
    MMPS_SYSTEM_VIDEODECMODES *pModes;

    pModes = MMPS_VIDPLAY_GetConfig();
    
    pModes->deblock_type = MMPS_VIDPLAY_DEBLOCK_NONE;

    // 3GPPLAY mode standard LCD out, fit width for demo
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.usX                  = 0;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.usY                  = 0;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.usWidth              = RTNA_LCD_GetAttr()->usPanelW;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.usHeight             = RTNA_LCD_GetAttr()->usPanelH;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.bMirror              = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.rotationtype         = MMP_DISPLAY_ROTATE_NO_ROTATE;

    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        pModes->config[VIDEOPLAY_MODE_STD_LCD].display.outputpanel      = MMP_DISPLAY_P_LCD;
        break;
    case LCD_TYPE_PLCD_FLM:
        pModes->config[VIDEOPLAY_MODE_STD_LCD].display.outputpanel      = MMP_DISPLAY_P_LCD_FLM;
        break;
    case LCD_TYPE_RGBLCD:
        pModes->config[VIDEOPLAY_MODE_STD_LCD].display.outputpanel      = MMP_DISPLAY_RGB_LCD;
        break;
    default:
        pModes->config[VIDEOPLAY_MODE_STD_LCD].display.outputpanel      = MMP_DISPLAY_NONE;
        break;
    }

    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.bFullyFilled         = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.bKeepRatio           = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_STD_LCD].display.bUseScaler           = MMP_TRUE;

    // 3GPPLAY mode standard LCD out, custom display
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.usX           = 0;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.usY           = 0;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.usWidth       = RTNA_LCD_GetAttr()->usPanelW;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.usHeight      = RTNA_LCD_GetAttr()->usPanelH;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.bMirror       = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.rotationtype  = MMP_DISPLAY_ROTATE_RIGHT_90;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.outputpanel   = MMP_DISPLAY_P_LCD;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.bFullyFilled  = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.bKeepRatio    = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_FULLSCREEN_LCD].display.bUseScaler    = MMP_TRUE;
    
    // 3GPPLAY mode standard LCD out, Rotate 180
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.usX                 = 0;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.usY                 = 0;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.usWidth             = MMPS_Display_GetConfig()->mainlcd.usWidth;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.usHeight            = MMPS_Display_GetConfig()->mainlcd.usHeight;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.bMirror             = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.rotationtype        = MMP_DISPLAY_ROTATE_RIGHT_180;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.outputpanel         = MMP_DISPLAY_P_LCD;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.bFullyFilled        = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.bKeepRatio          = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_ZOOM_LCD].display.bUseScaler          = MMP_TRUE;
    
    // 3GPPLAY mode standard LCD out, Rotate 270
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.usX               = 0;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.usY               = 0;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.usWidth           = MMPS_Display_GetConfig()->mainlcd.usWidth;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.usHeight          = MMPS_Display_GetConfig()->mainlcd.usHeight;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.bMirror           = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.rotationtype      = MMP_DISPLAY_ROTATE_RIGHT_270;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.outputpanel       = MMP_DISPLAY_P_LCD;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.bFullyFilled      = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.bKeepRatio        = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_ACTUAL_LCD].display.bUseScaler        = MMP_FALSE;

    // Standard TV out, fit width for demo
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.usX                  = 0;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.usY                  = 0;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.usWidth              = 720;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.usHeight             = 480;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.bMirror              = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.rotationtype         = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.outputpanel          = MMP_DISPLAY_TV;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.bFullyFilled         = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.bKeepRatio           = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_NTSC_TV].display.bUseScaler           = MMP_TRUE;

    // Standard TV out, fit width for demo
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.usX                   = 0;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.usY                   = 0;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.usWidth               = 720;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.usHeight              = 576;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.bMirror               = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.rotationtype          = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.outputpanel           = MMP_DISPLAY_TV;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.bFullyFilled          = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.bKeepRatio            = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_PAL_TV].display.bUseScaler            = MMP_TRUE;
    
    // HDMI output
    pModes->config[VIDEOPLAY_MODE_HDMI].display.usX                     = 0;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.usY                     = 0;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.usWidth                 = MMPS_Display_GetConfig()->hdmi.usDisplayWidth;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.usHeight                = MMPS_Display_GetConfig()->hdmi.usDisplayHeight;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.bMirror                 = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.rotationtype            = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.outputpanel             = MMP_DISPLAY_HDMI;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.bFullyFilled            = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.bKeepRatio              = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_HDMI].display.bUseScaler              = MMP_TRUE; // For 720P display to 1080P HDMI.
    
    // CCIR output
    pModes->config[VIDEOPLAY_MODE_CCIR].display.usX                     = 0;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.usY                     = 0;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.usWidth                 = 720;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.usHeight                = 480;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.bMirror                 = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.rotationtype            = MMP_DISPLAY_ROTATE_NO_ROTATE;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.outputpanel             = MMP_DISPLAY_CCIR;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.bFullyFilled            = MMP_FALSE;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.bKeepRatio              = MMP_TRUE;
    pModes->config[VIDEOPLAY_MODE_CCIR].display.bUseScaler              = MMP_TRUE;

    // Buffer size for video stream
    pModes->ulVideoStreamBufSize    = 2 * 1024 * 1024;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
    pModes->ulRotateDMABufNum       = 0;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
    pModes->ulRotateDMABufNum       = 3;
#endif

    pModes->usMP4DecBufNum          = 6;  // must >= 3
    pModes->window_type             = MMP_DISPLAY_WIN_PIP;
    pModes->videoplay_mode          = VIDEOPLAY_MODE_STD_LCD;

#endif
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_Display_InitConfig
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPC_Display_InitConfig(void)
{
    switch (RTNA_LCD_GetAttr()->ubDevType)
    {
    case LCD_TYPE_PLCD:
        MMPS_Display_GetConfig()->mainlcd.lcdtype           = MMP_DISPLAY_P_LCD;
        MMPS_Display_GetConfig()->mainlcd.cssel             = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->mainlcd.initialfx         = MMPC_Display_InitMainLCD;
        MMPS_Display_GetConfig()->mainlcd.reinitialfx       = MMPC_Display_ReinitMainLCD;
        MMPS_Display_GetConfig()->mainlcd.usWidth           = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->mainlcd.usHeight          = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->mainlcd.colordepth        = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->mainlcd.ulBgColor         = 0x000000;

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        if (LCD_RATIO_16_9 == RTNA_LCD_GetAttr()->ubRatio) {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 16;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 9;
        }
        else {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 4;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 3;
        }
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        if (LCD_RATIO_16_9 == RTNA_LCD_GetAttr()->ubRatio) {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 9;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 16;
        }
        else {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 3;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 4;
        }     
#endif        
        
        MMPS_Display_GetConfig()->mainlcd.bFLMType          = MMP_FALSE;        
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_FALSE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_TRUE;
#endif

        MMPS_Display_GetConfig()->sublcd.lcdtype            = MMP_DISPLAY_P_LCD;
        MMPS_Display_GetConfig()->sublcd.cssel              = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->sublcd.initialfx          = NULL;
        MMPS_Display_GetConfig()->sublcd.reinitialfx        = MMPC_Display_ReinitMainLCD;
        MMPS_Display_GetConfig()->sublcd.usWidth            = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->sublcd.usHeight           = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->sublcd.colordepth         = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->sublcd.ulBgColor          = 0xBB0000;
        MMPS_Display_GetConfig()->sublcd.ulPanelXratio      = 4;
        MMPS_Display_GetConfig()->sublcd.ulPanelYratio      = 3;
        MMPS_Display_GetConfig()->sublcd.bFLMType           = MMP_FALSE;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_FALSE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_TRUE;
#endif
        
        break;
    case LCD_TYPE_PLCD_FLM:
        MMPS_Display_GetConfig()->mainlcd.lcdtype           = MMP_DISPLAY_P_LCD_FLM;
        MMPS_Display_GetConfig()->mainlcd.cssel             = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->mainlcd.initialfx         = MMPC_Display_InitMainLCD;
        MMPS_Display_GetConfig()->mainlcd.reinitialfx       = MMPC_Display_ReinitMainLCD;
        MMPS_Display_GetConfig()->mainlcd.usWidth           = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->mainlcd.usHeight          = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->mainlcd.colordepth        = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->mainlcd.ulBgColor         = 0x000000;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        if (LCD_RATIO_16_9 == RTNA_LCD_GetAttr()->ubRatio) {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 16;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 9;
        }
        else {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 4;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 3;
        }
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        if (LCD_RATIO_16_9 == RTNA_LCD_GetAttr()->ubRatio) {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 9;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 16;
        }
        else {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 3;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 4;
        }                  
#endif

        MMPS_Display_GetConfig()->mainlcd.bFLMType          = MMP_TRUE;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_FALSE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_TRUE;             
#endif

        MMPS_Display_GetConfig()->sublcd.lcdtype            = MMP_DISPLAY_P_LCD;
        MMPS_Display_GetConfig()->sublcd.cssel              = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->sublcd.initialfx          = NULL;
        MMPS_Display_GetConfig()->sublcd.reinitialfx        = MMPC_Display_ReinitMainLCD;
        MMPS_Display_GetConfig()->sublcd.usWidth            = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->sublcd.usHeight           = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->sublcd.colordepth         = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->sublcd.ulBgColor          = 0xBB0000;
        MMPS_Display_GetConfig()->sublcd.ulPanelXratio      = 4;
        MMPS_Display_GetConfig()->sublcd.ulPanelYratio      = 3;
        MMPS_Display_GetConfig()->sublcd.bFLMType           = MMP_TRUE;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_FALSE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_TRUE;             
#endif        
        break;
    case LCD_TYPE_RGBLCD:
        MMPS_Display_GetConfig()->mainlcd.lcdtype           = MMP_DISPLAY_RGB_LCD;
        MMPS_Display_GetConfig()->mainlcd.cssel             = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->mainlcd.initialfx         = MMPC_Display_InitMainLCD;
        MMPS_Display_GetConfig()->mainlcd.reinitialfx       = MMPC_Display_ReinitMainLCD;
        MMPS_Display_GetConfig()->mainlcd.usWidth           = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->mainlcd.usHeight          = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->mainlcd.colordepth        = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->mainlcd.ulBgColor         = 0x000000;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        if (LCD_RATIO_16_9 == RTNA_LCD_GetAttr()->ubRatio) {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 16;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 9;
        }
        else {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 4;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 3;
        }
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        if (LCD_RATIO_16_9 == RTNA_LCD_GetAttr()->ubRatio) {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 9;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 16;
        }
        else {
            MMPS_Display_GetConfig()->mainlcd.ulPanelXratio = 3;
            MMPS_Display_GetConfig()->mainlcd.ulPanelYratio = 4;
        }              
#endif

        MMPS_Display_GetConfig()->mainlcd.bFLMType          = MMP_FALSE;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_FALSE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
          MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_TRUE;           
#endif        
        MMPS_Display_GetConfig()->sublcd.lcdtype            = MMP_DISPLAY_RGB_LCD;
        MMPS_Display_GetConfig()->sublcd.cssel              = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->sublcd.initialfx          = NULL;
        MMPS_Display_GetConfig()->sublcd.reinitialfx        = MMPC_Display_ReinitMainLCD;
        MMPS_Display_GetConfig()->sublcd.usWidth            = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->sublcd.usHeight           = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->sublcd.colordepth         = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->sublcd.ulBgColor          = 0xBB0000; 
        MMPS_Display_GetConfig()->sublcd.ulPanelXratio      = 4;
        MMPS_Display_GetConfig()->sublcd.ulPanelYratio      = 3;
        MMPS_Display_GetConfig()->sublcd.bFLMType           = MMP_FALSE;
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_FALSE;
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_FALSE;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_TRUE;             
#endif        
        break;
    default:
        MMPS_Display_GetConfig()->mainlcd.lcdtype           = MMP_DISPLAY_NONE;
        MMPS_Display_GetConfig()->mainlcd.cssel             = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->mainlcd.initialfx         = NULL;
        MMPS_Display_GetConfig()->mainlcd.reinitialfx       = NULL;
        MMPS_Display_GetConfig()->mainlcd.usWidth           = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->mainlcd.usHeight          = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->mainlcd.colordepth        = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->mainlcd.ulBgColor         = 0x000000;
        MMPS_Display_GetConfig()->mainlcd.ulPanelXratio     = LCD_MODEL_RATIO_X;
        MMPS_Display_GetConfig()->mainlcd.ulPanelYratio     = LCD_MODEL_RATIO_Y;
        MMPS_Display_GetConfig()->mainlcd.bFLMType          = MMP_FALSE;
        MMPS_Display_GetConfig()->mainlcd.bRotateVerticalLCD = MMP_FALSE;
        
        MMPS_Display_GetConfig()->sublcd.lcdtype            = MMP_DISPLAY_NONE;
        MMPS_Display_GetConfig()->sublcd.cssel              = MMP_LCD_CS_SEL_1;
        MMPS_Display_GetConfig()->sublcd.initialfx          = NULL;
        MMPS_Display_GetConfig()->sublcd.reinitialfx        = NULL;
        MMPS_Display_GetConfig()->sublcd.usWidth            = RTNA_LCD_GetAttr()->usPanelW;
        MMPS_Display_GetConfig()->sublcd.usHeight           = RTNA_LCD_GetAttr()->usPanelH;
        MMPS_Display_GetConfig()->sublcd.colordepth         = MMP_LCD_COLORDEPTH_16;
        MMPS_Display_GetConfig()->sublcd.ulBgColor          = 0x000000; 
        MMPS_Display_GetConfig()->sublcd.ulPanelXratio      = 4;
        MMPS_Display_GetConfig()->sublcd.ulPanelYratio      = 3;
        MMPS_Display_GetConfig()->sublcd.bFLMType           = MMP_FALSE;
        MMPS_Display_GetConfig()->sublcd.bRotateVerticalLCD = MMP_FALSE;
        break;
    }
    
    MMPS_Display_GetConfig()->ntsctv.tvtype             = MMP_TV_TYPE_NTSC;
    MMPS_Display_GetConfig()->ntsctv.usStartX           = 0;
    MMPS_Display_GetConfig()->ntsctv.usStartY           = 0;
    MMPS_Display_GetConfig()->ntsctv.usDisplayWidth     = 720;
    MMPS_Display_GetConfig()->ntsctv.usDisplayHeight    = 480;
    MMPS_Display_GetConfig()->ntsctv.ulDspyBgColor      = 0x00;
    MMPS_Display_GetConfig()->ntsctv.ubTvBgYColor       = 0x00;
    MMPS_Display_GetConfig()->ntsctv.ubTvBgUColor       = 0x80;
    MMPS_Display_GetConfig()->ntsctv.ubTvBgVColor       = 0x80;

    MMPS_Display_GetConfig()->paltv.tvtype              = MMP_TV_TYPE_PAL;
    MMPS_Display_GetConfig()->paltv.usStartX            = 0;
    MMPS_Display_GetConfig()->paltv.usStartY            = 0;
    MMPS_Display_GetConfig()->paltv.usDisplayWidth      = 720;
    MMPS_Display_GetConfig()->paltv.usDisplayHeight     = 576;
    MMPS_Display_GetConfig()->paltv.ulDspyBgColor       = 0x00;
    MMPS_Display_GetConfig()->paltv.ubTvBgYColor        = 0x00;
    MMPS_Display_GetConfig()->paltv.ubTvBgUColor        = 0x80;
    MMPS_Display_GetConfig()->paltv.ubTvBgVColor        = 0x80;

    MMPS_Display_GetConfig()->hdmi.ubColorType          = MMP_HDMI_COLORMODE_RGB888;
    MMPS_Display_GetConfig()->hdmi.usDisplayWidth       = 1920;
    MMPS_Display_GetConfig()->hdmi.usDisplayHeight      = 1080;
    MMPS_Display_GetConfig()->hdmi.ulDspyBgColor        = 0x000000;
    MMPS_Display_GetConfig()->hdmi.ubOutputMode         = MMP_HDMI_OUTPUT_1920X1080P;

    // ubOutputColor is needed same as ubColorType while ubColorType is not yuv420)
    MMPS_Display_GetConfig()->ccir.ubCCIRMode           = MMP_CCIR_656_NTSC; //only support CCIR_656_NTSC & MMP_CCIR_601_NTSC
    MMPS_Display_GetConfig()->ccir.ubColorType          = MMP_CCIR656_YUV422_8BIT_UYVY;
    MMPS_Display_GetConfig()->ccir.usDisplayWidth       = 720;
    MMPS_Display_GetConfig()->ccir.usDisplayHeight      = 480;
    if(MMPS_Display_GetConfig()->ccir.ubCCIRMode == MMP_CCIR_656_NTSC) {
        MMPS_Display_GetConfig()->ccir.ubClockDiv           = 5;
        MMPS_Display_GetConfig()->ccir.ubHSyncClkInv        = MMP_FALSE;
        MMPS_Display_GetConfig()->ccir.ubVSyncClkInv        = MMP_FALSE;
    }
    else if (MMPS_Display_GetConfig()->ccir.ubCCIRMode == MMP_CCIR_601_NTSC) {
        MMPS_Display_GetConfig()->ccir.ubClockDiv           = 10;
        MMPS_Display_GetConfig()->ccir.ubHSyncClkInv        = MMP_TRUE;
        MMPS_Display_GetConfig()->ccir.ubVSyncClkInv        = MMP_FALSE;
    }
    MMPS_Display_GetConfig()->OutputPanel               = MMP_DISPLAY_SEL_MAIN_LCD;

    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_Audio_InitConfig
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPC_Audio_InitConfig(void)
{
#if !defined(MBOOT_FW)
    MMP_UBYTE i, j;

    #if 1
    MMPS_Audio_GetConfig()->AudioOutPath        = MMPS_AUDIO_OUT_AFE_HP | MMPS_AUDIO_OUT_AFE_LINE;
    #endif
    #if 0
    MMPS_Audio_GetConfig()->AudioOutPath        = MMPS_AUDIO_OUT_I2S;
    MMPS_Audio_GetConfig()->AudioOutI2SFormat   = MMPS_AUDIO_I2S_MASTER;
    #endif

    #if 1
    MMPS_Audio_GetConfig()->AudioInPath         = MMPS_AUDIO_IN_AFE_DIFF;
    #endif
    #if 0
    MMPS_Audio_GetConfig()->AudioInPath         = MMPS_AUDIO_IN_I2S0;
    MMPS_Audio_GetConfig()->AudioInI2SFormat    = MMPS_AUDIO_I2S_MASTER;
    #endif
    
    MMPS_AUDIO_InitialInPath(MMPS_Audio_GetConfig()->AudioInPath);
    MMPS_AUDIO_InitialOutPath(MMPS_Audio_GetConfig()->AudioOutPath);

    #if (AUDIO_SET_DB == 0x0)
    MMPS_Audio_GetConfig()->ubVolumeLevel       = 31;
    #else
    MMPS_Audio_GetConfig()->ubVolumeLevel       = 31;
    #endif

    for(i = 0; i < MMPS_Audio_GetConfig()->ubVolumeLevel; i++) {
        for(j=0; j < 3; j++) {
            MMPS_Audio_GetConfig()->ubAudioOutVolHPTable[i][j] = ubAudOutVol_HP_table[i][j];
        }
    }
    for(i = 0; i < MMPS_Audio_GetConfig()->ubVolumeLevel; i++) {
        for(j=0; j < 2; j++) {
            MMPS_Audio_GetConfig()->ubAudioOutVolLineTable[i][j] = ubAudOutVol_Line_table[i][j];
        }       
    }
    
    //SBC setting
    MMPS_Audio_GetConfig()->SBC.ChannelMode         = MMPS_AUDIO_SBC_CHANNEL_STEREO;
    MMPS_Audio_GetConfig()->SBC.AllocMethod         = MMPS_AUDIO_SBC_METHOD_SNR;
    MMPS_Audio_GetConfig()->SBC.usNrofBlocks        = 16;
    MMPS_Audio_GetConfig()->SBC.usNrofSubbands      = 8;
    MMPS_Audio_GetConfig()->SBC.ulBitRate           = 350;
    MMPS_Audio_GetConfig()->SBC.ulIntPeriod         = 200;
    MMPS_Audio_GetConfig()->SBC.ulMaxTransferSize   = 16*1024;
    
    MMPS_Audio_GetConfig()->ulNameBufSize           = 0x200;
    MMPS_Audio_GetConfig()->ulParameterBufSize      = 0x400;
    MMPS_Audio_GetConfig()->ulSdBufSize             = 0x400;
    MMPS_Audio_GetConfig()->ulSmBufSize             = 0x400;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_CARD_MODE].ulBufStartAddr = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_CARD_MODE].ulPlayBufSize  = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_CARD_MODE].ulSbcBufSize   = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_CARD_MODE].ulParseBufSize = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_MEM_MODE].ulBufStartAddr = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_MEM_MODE].ulPlayBufSize  = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_MEM_MODE].ulSbcBufSize   = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AACPLUS_P_MEM_MODE].ulParseBufSize = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_CARD_MODE].ulBufStartAddr   = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_CARD_MODE].ulPlayBufSize    = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_CARD_MODE].ulSbcBufSize     = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_CARD_MODE].ulParseBufSize   = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_MEM_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_MEM_MODE].ulPlayBufSize     = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_MEM_MODE].ulPlayBufSize     = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_FLAC_P_MEM_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MIDI_P_CARD_MODE].ulBufStartAddr   = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MIDI_P_CARD_MODE].ulPlayBufSize    = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MIDI_P_CARD_MODE].ulParseBufSize   = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MIDI_P_MEM_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MIDI_P_MEM_MODE].ulPlayBufSize     = 0x10000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MIDI_P_MEM_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_PCM_P_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_PCM_P_MEM_MODE].ulPlayBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_PCM_P_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_CARD_MODE].ulPlayBufSize     = 0x4000;
    
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_CARD_MODE].ulSbcBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_CARD_MODE].ulWavBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_CARD_MODE].ulParseBufSize    = 0x4000;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_MEM_MODE].ulPlayBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_MEM_MODE].ulSbcBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_MEM_MODE].ulWavBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_P_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_CARD_MODE].ulTagBufSize      = 0x200;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_CARD_MODE].ulPlayBufSize     = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_CARD_MODE].ulSbcBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_MEM_MODE].ulTagBufSize       = 0x200;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_MEM_MODE].ulPlayBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_MEM_MODE].ulSbcBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WMA_P_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_CARD_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_CARD_MODE].ulTagBufSize       = 0x200;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_CARD_MODE].ulPlayBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_CARD_MODE].ulSbcBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_CARD_MODE].ulParseBufSize     = 0;
    
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_MEM_MODE].ulBufStartAddr      = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_MEM_MODE].ulTagBufSize        = 0x200;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_MEM_MODE].ulPlayBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_MEM_MODE].ulSbcBufSize        = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_RA_P_MEM_MODE].ulParseBufSize      = 0;
    
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_CARD_MODE].ulTagBufSize      = 0x200;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_CARD_MODE].ulPlayBufSize     = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_CARD_MODE].ulSbcBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_CARD_MODE].ulParseBufSize    = 0;
    
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_MEM_MODE].ulTagBufSize       = 0x200;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_MEM_MODE].ulPlayBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_MEM_MODE].ulSbcBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_OGG_P_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_CARD_MODE].ulPlayBufSize     = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_CARD_MODE].ulSbcBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_MEM_MODE].ulPlayBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_MEM_MODE].ulSbcBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_P_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_CARD_MODE].ulPlayBufSize     = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_CARD_MODE].ulSbcBufSize      = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_MEM_MODE].ulPlayBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_MEM_MODE].ulSbcBufSize       = 0x8000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_P_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_R_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_R_CARD_MODE].ulRecBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_R_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_R_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_R_MEM_MODE].ulRecBufSize       = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AMR_R_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AAC_R_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AAC_R_CARD_MODE].ulRecBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AAC_R_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AAC_R_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AAC_R_MEM_MODE].ulRecBufSize       = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_AAC_R_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_R_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_R_CARD_MODE].ulRecBufSize      = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_R_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_R_MEM_MODE].ulBufStartAddr     = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_R_MEM_MODE].ulRecBufSize       = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_MP3_R_MEM_MODE].ulParseBufSize     = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_R_CARD_MODE].ulBufStartAddr    = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_R_CARD_MODE].ulRecBufSize      = 0xC000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUDIO_WAV_R_CARD_MODE].ulParseBufSize    = 0;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUI_MP3_P_CARD_MODE].ulBufStartAddr      = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUI_MP3_P_CARD_MODE].ulPlayBufSize       = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUI_MP3_P_CARD_MODE].ulParseBufSize      = 0x4000;

    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUI_WAV_P_CARD_MODE].ulBufStartAddr      = 0;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUI_WAV_P_CARD_MODE].ulPlayBufSize       = 0x4000;
    MMPS_Audio_GetConfig()->AudioMode[MMPS_AUI_WAV_P_CARD_MODE].ulParseBufSize      = 0;
#endif
    return MMP_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : MMPC_System_InitConfig
//  Description :
//------------------------------------------------------------------------------
MMP_BOOL MMPC_System_InitConfig(void)
{
    MMPS_SYSTEM_CONFIG *pConfig = MMPS_System_GetConfig();

    pConfig->ulStackMemoryStart     = 0x1000000;
    pConfig->ulMaxReserveBufferSize = 96*1024;
    pConfig->ulMaxReserveChannel    = 2;
#ifdef MCR_V2_32MB
    pConfig->ulStackMemorySize      = 32*1024*1024;
#else
    pConfig->ulStackMemorySize      = 64*1024*1024;
#endif

    #if 1 //DDR
    pConfig->stackMemoryType        = MMPS_DRAM_TYPE_AUTO;
    pConfig->stackMemoryMode        = MMPS_DRAM_MODE_DDR;
    #endif
    #if 0 //DDR2
    pConfig->stackMemoryType        = MMPS_DRAM_TYPE_EXT;
    pConfig->stackMemoryMode        = MMPS_DRAM_MODE_DDR2;
    #endif

    return MMP_TRUE;
}

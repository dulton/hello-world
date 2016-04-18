//==============================================================================
//
//  File        : mmp_aud_inc.h
//  Description : INCLUDE File for Audio Function
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_AUD_INC_H_
#define _MMP_AUD_INC_H_

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================

typedef void (*MMP_LivePCMCB) (void*, MMP_ULONG);

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum MMP_AUDIO_CODEC{
    MMP_AUDIO_CODEC_MP3,
    MMP_AUDIO_CODEC_AMR,
    MMP_AUDIO_CODEC_AAC,
    MMP_AUDIO_CODEC_WMA,
    MMP_AUDIO_CODEC_OGG,
    MMP_AUDIO_CODEC_RA,
    MMP_AUDIO_CODEC_WAV,
    MMP_AUDIO_CODEC_MIDI,
    MMP_AUDIO_CODEC_RAWPCM,
    MMP_AUDIO_CODEC_G711,
    MMP_AUDIO_CODEC_INVALID
} MMP_AUDIO_CODEC;

// Audio input channel, it depends on the board schematic
typedef enum _MMP_AUD_LINEIN_CH {
    MMP_AUD_LINEIN_DUAL = 0x0,  ///< Dual channel line in
    MMP_AUD_LINEIN_R    = 0x1,  ///< Right channel line in
    MMP_AUD_LINEIN_L    = 0x2,  ///< Left channel line in
    MMP_AUD_LINEIN_SWAP = 0x3   ///< Dual channel, L and R swap
} MMP_AUD_LINEIN_CH;

// Audio input/output path
typedef enum _MMP_AUD_INOUT_PATH {
    /* AFE input path */
    MMP_AUD_AFE_IN              = 0x000001,
    MMP_AUD_AFE_IN_SING         = 0x000002, ///< AUXL/AUXR
    MMP_AUD_AFE_IN_DIFF         = 0x000004, ///< LIP/LIN/RIP/RIN
    MMP_AUD_AFE_IN_DIFF2SING    = 0x000008, ///< LIP/RIP
    MMP_AUD_AFE_IN_MASK         = 0x00000F,
    /* AFE output path */
    MMP_AUD_AFE_OUT             = 0x000100,
    MMP_AUD_AFE_OUT_HP          = 0x000200, ///< HP_OUT (LOUT/ROUT)
    MMP_AUD_AFE_OUT_HP_INVT     = 0x000400, ///< HP_OUT Inverter (LOUT/ROUT)
    MMP_AUD_AFE_OUT_LINE        = 0x000800, ///< LINE_OUT (LOUT2/ROUT2)
    MMP_AUD_AFE_OUT_LINE_INVT   = 0x001000, ///< LINE_OUT Inverter (LOUT2/ROUT2)
    MMP_AUD_AFE_OUT_MASK        = 0x001F00,
    /* I2S path */
    MMP_AUD_I2S_INOUT           = 0x010000,
    MMP_AUD_I2S0                = 0x020000, ///< I2S channel 0
    MMP_AUD_I2S1                = 0x040000, ///< I2S channel 1
    MMP_AUD_I2S2                = 0x080000, ///< I2S channel 2
    MMP_AUD_I2S_MASK            = 0x0F0000,
        #define I2S_PATH(ch)    (1 << (ch + 4))
    MMP_AUD_DMIC_PAD0			= 0x100000,
    MMP_AUD_DMIC_PAD1           = 0x200000,
    MMP_AUD_DMIC_PAD2           = 0x400000,
    MMP_AUD_DMIC_MASK           = 0xF00000
} MMP_AUD_INOUT_PATH;

typedef enum _MMP_AUD_HPF_MODE {
    MMP_AUD_HPF_AUD_2HZ=0,
    MMP_AUD_HPF_AUD_4HZ,
    MMP_AUD_HPF_AUD_8HZ,
    MMP_AUD_HPF_AUD_16HZ,
    MMP_AUD_HPF_VOC_2D5HZ,
    MMP_AUD_HPF_VOC_25HZ,
    MMP_AUD_HPF_VOC_50HZ,
    MMP_AUD_HPF_VOC_100HZ,
    MMP_AUD_HPF_VOC_200HZ,
    MMP_AUD_HPF_VOC_300HZ,
    MMP_AUD_HPF_VOC_400HZ,
    MMP_AUD_HPF_BYPASS 
}MMP_AUD_HPF_MODE;

#endif //_MMP_AUD_INC_H_
#ifndef MMPF_AUDIO_CTL_H
#define MMPF_AUDIO_CTL_H

#include "config_fw.h"
#include "mmp_aud_inc.h"

#if (AUDIO_AMR_R_EN)
#include "mmpf_amrencapi.h"
#endif
#if (AUDIO_AAC_R_EN)
#include "mmpf_aacencapi.h"
#endif
#if (VADPCM_R_EN)
#include "mmpf_adpcmencapi.h"
#endif
#if (WAV_R_EN)||(VPCM_R_EN)
#include "mmpf_wavencapi.h"
#endif
#if (AUDIO_MP3_R_EN)
#include "mmpf_mp3encapi.h"
#endif
#if (AUDIO_AMR_P_EN)
#include "mmpf_amrdecapi.h"
#endif
#if (WAV_P_EN)||(VWAV_P_EN)
#include "mmpf_wavdecapi.h"
#endif
#if (AUDIO_MP3_P_EN)
#include "mmpf_mp3api.h"
#endif
#if (AUDIO_AAC_P_EN)
#include "mmpf_aacplusdecapi.h"
#endif
#if (RA_EN)||(VRA_P_EN)
#include "mmpf_radecapi.h"
#endif
#if (OGG_EN)
#include "mmpf_oggdecapi.h"
#endif
#if (WMAPRO10_EN)||(VWMA_P_EN)
#include "mmpf_wmaprodecapi.h"
#endif
#if (FLAC_P_EN)
#include "mmpf_flacdecapi.h"
#endif
#if (MP3HD_P_EN)
#include "mmpf_mp3hdapi.h"
#endif
#if (WNR_EN||NR_EN)&&((AUDIO_R_EN)||(VIDEO_R_EN))
#include "mmpf_wnr.h"
#include "mmpf_nr.h"
#endif
#if (G711_MU_R_EN)
#include "mmpf_g711encapi.h"
#endif
#if (LIVE_PCM_R_EN)
#include "mmpf_pcmencapi.h"
#endif

/* Audio encode buffer size */
#if (AUDIO_AMR_R_EN)
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  AMRENC_BUFFER_SIZE
#endif
#if (VADPCM_R_EN)
#ifdef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #if (ADPCMENC_BUFFER_SIZE > AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  ADPCMENC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  ADPCMENC_BUFFER_SIZE
#endif
#endif
#if (WAV_R_EN)
#ifdef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #if (WAVENC_BUFFER_SIZE > AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  WAVENC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  WAVENC_BUFFER_SIZE
#endif
#endif
#if (AUDIO_MP3_R_EN)
#ifdef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #if (MP3ENC_BUFFER_SIZE > AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  MP3ENC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  MP3ENC_BUFFER_SIZE
#endif
#endif
#if (AUDIO_AAC_R_EN)
#ifdef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #if (AACENC_BUFFER_SIZE > AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  AACENC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF  AACENC_BUFFER_SIZE
#endif
#endif

/* Audio decode buffer size */
#if (AUDIO_AMR_P_EN)
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  AMRDEC_BUFFER_SIZE
#endif
#if (WAV_P_EN)||(VWAV_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (WAVDEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  WAVDEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  WAVDEC_BUFFER_SIZE
#endif
#endif
#if (AUDIO_MP3_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (MP3DEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  MP3DEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  MP3DEC_BUFFER_SIZE
#endif
#endif
#if (AUDIO_AAC_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (AACPLUSDEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  AACPLUSDEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  AACPLUSDEC_BUFFER_SIZE
#endif
#endif
#if (RA_EN)||(VRA_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (RADEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  RADEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  RADEC_BUFFER_SIZE
#endif
#endif
#if (OGG_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (OGGDEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  OGGDEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  OGGDEC_BUFFER_SIZE
#endif
#endif
#if (WMAPRO10_EN)||(VWMA_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (WMAPRODEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  WMAPRODEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  WMAPRODEC_BUFFER_SIZE
#endif
#endif
#if (FLAC_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (FLACDEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  FLACDEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  FLACDEC_BUFFER_SIZE
#endif
#endif
#if (MP3HD_P_EN)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (MP3HDDEC_BUFFER_SIZE > AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF)
    #undef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  MP3HDDEC_BUFFER_SIZE
    #endif
#else
    #define AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF  MP3HDDEC_BUFFER_SIZE
#endif
#endif

/* Audio record effect buffer size */
#if (WNR_EN||NR_EN)&&((AUDIO_R_EN)||(VIDEO_R_EN))
    #define AUDIO_ENC_TOTAL_WORKBUF_SIZE  (AUDIO_ENC_SIZEOF_TOTAL_WORKING_BUF + WNR_WORKBUF_SIZE + NR_WORKBUF_SIZE)
#if (AUDIO_DEC_ENC_SHARE_WB)
#ifdef AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #if (AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF < AUDIO_ENC_TOTAL_WORKBUF_SIZE)
    #define AUDIO_ENC_SIZEOF_TOTAL_FINAL_WORKING_BUF    AUDIO_ENC_TOTAL_WORKBUF_SIZE
    #else
    #define AUDIO_ENC_SIZEOF_TOTAL_FINAL_WORKING_BUF    AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
    #endif
#endif //AUDIO_DEC_SIZEOF_TOTAL_WORKING_BUF
#else
    #define AUDIO_ENC_SIZEOF_TOTAL_FINAL_WORKING_BUF    AUDIO_ENC_TOTAL_WORKBUF_SIZE
#endif //(AUDIO_DEC_ENC_SHARE_WB)
#endif //(WNR_EN||NR_EN)&&((AUDIO_R_EN)||(VIDEO_R_EN))

#if (G711_MU_R_EN)
#ifdef AUDIO_LIVE_SIZEOF_IN_BUF
    #if ((G711_INBUF_SIZE << 1) > AUDIO_LIVE_SIZEOF_IN_BUF)
    #undef AUDIO_LIVE_SIZEOF_IN_BUF
    #define AUDIO_LIVE_SIZEOF_IN_BUF    (G711_INBUF_SIZE << 1)
    #endif
#else
    #define AUDIO_LIVE_SIZEOF_IN_BUF    (G711_INBUF_SIZE << 1)
#endif
#ifdef AUDIO_LIVE_SIZEOF_OUT_BUF
    #if (G711_OUTBUF_SIZE > AUDIO_LIVE_SIZEOF_OUT_BUF)
    #undef AUDIO_LIVE_SIZEOF_OUT_BUF
    #define AUDIO_LIVE_SIZEOF_OUT_BUF   G711_OUTBUF_SIZE
    #endif
#else
    #define AUDIO_LIVE_SIZEOF_OUT_BUF   G711_OUTBUF_SIZE
#endif
#endif //(G711_MU_R_EN)

#if (LIVE_PCM_R_EN)
#ifdef AUDIO_LIVE_SIZEOF_IN_BUF
    #if (LIVE_PCM_INBUF_SIZE > AUDIO_LIVE_SIZEOF_IN_BUF)
    #undef AUDIO_LIVE_SIZEOF_IN_BUF
    #define AUDIO_LIVE_SIZEOF_IN_BUF    (LIVE_PCM_INBUF_SIZE << 1)
    #endif
#else
    #define AUDIO_LIVE_SIZEOF_IN_BUF    (LIVE_PCM_INBUF_SIZE << 1)
#endif
#ifdef AUDIO_LIVE_SIZEOF_OUT_BUF
    #if (LIVE_PCM_OUTBUF_SIZE > AUDIO_LIVE_SIZEOF_OUT_BUF)
    #undef AUDIO_LIVE_SIZEOF_OUT_BUF
    #define AUDIO_LIVE_SIZEOF_OUT_BUF   LIVE_PCM_OUTBUF_SIZE
    #endif
#else
    #define AUDIO_LIVE_SIZEOF_OUT_BUF   LIVE_PCM_OUTBUF_SIZE
#endif
#endif //(LIVE_PCM_R_EN)

#if (LIVE_AAC_R_EN)
#ifdef AUDIO_LIVE_SIZEOF_IN_BUF
#else
    #define AUDIO_LIVE_SIZEOF_IN_BUF    (0)
#endif
#ifdef AUDIO_LIVE_SIZEOF_OUT_BUF
    #if (LIVE_AAC_OUTBUF_SIZE > AUDIO_LIVE_SIZEOF_OUT_BUF)
    #undef AUDIO_LIVE_SIZEOF_OUT_BUF
    #define AUDIO_LIVE_SIZEOF_OUT_BUF   LIVE_AAC_OUTBUF_SIZE
    #endif
#else
    #define AUDIO_LIVE_SIZEOF_OUT_BUF   LIVE_AAC_OUTBUF_SIZE
#endif
#endif //(LIVE_AAC_R_EN)


extern	int	*glAudioEncWorkingBuf;
extern	int	*glAudioDecWorkingBuf;

#if (LIVE_AAC_R_EN)||(G711_MU_R_EN)||(LIVE_PCM_R_EN)
extern	int	glAudioLiveInBuf[AUDIO_LIVE_SIZEOF_IN_BUF >> 2];
extern	int	glAudioLiveOutBuf[AUDIO_LIVE_SIZEOF_OUT_BUF >> 2];
#endif

#define AUDIO_BYPASS_DISABLE 		0x00 ///< audio bypass disable
#define AUDIO_BYPASS_LL 			0x01 ///< audio bypass L-in to L-out
#define AUDIO_BYPASS_LR 			0x02 ///< audio bypass L-in to R-out
#define AUDIO_BYPASS_RL 			0x04 ///< audio bypass R-in to L-out
#define AUDIO_BYPASS_RR 			0x08 ///< audio bypass R-in to R-out

#define AUDIO_FIFO_DEPTH    512

typedef enum _MMPF_AUDIO_DATA_FLOW
{
    AFE_FIFO_TO_DAC = 0x1,  ///< AFE FIFO to AFE DAC
    ADC_TO_AFE_FIFO,        ///< AFE ADC to AFE FIFO
    ADC_TO_I2S0_FIFO,       ///< AFE ADC to I2S0 FIFO
    I2S0_FIFO_TO_SDO,       ///< I2S0 FIFO to I2S output
    SDI0_TO_AFE_FIFO,       ///< I2S0 input to AFE FIFO
    SDI_TO_I2S0_FIFO,       ///< I2S input to I2S0 FIFO
    #if (CHIP == MCR_V2)
    ADC_TO_I2S1_FIFO,       ///< AFE ADC to I2S1 FIFO
    I2S1_FIFO_TO_SDO,       ///< I2S1 FIFO to I2S output
    I2S2_FIFO_TO_SDO,       ///< I2S2 FIFO to I2S output
    SDI1_TO_AFE_FIFO,      	///< I2S1 input to AFE FIFO
    SDI_TO_I2S1_FIFO,      	///< I2S input to I2S1 FIFO
    /*++MCR_V2 MP ONLY*/
    I2S2_FULL_DUPLEX,    	///< I2S input to I2S2 FIFO
    I2S1_FULL_DUPLEX,    	///< I2S input to I2S2 FIFO
    I2S0_FULL_DUPLEX,   	///< I2S input to I2S2 FIFO
    AFE_FULL_DUPLEX,       ///< I2S input to I2S2 FIFO
    SDI_TO_I2S2_RX_FIFO,    ///< I2S input to I2S2 FIFO
    SDI_TO_I2S1_RX_FIFO,    ///< I2S input to I2S1 FIFO
    SDI_TO_I2S0_RX_FIFO,    ///< I2S input to I2S0 FIFO
    I2S2_TX_FIFO_TO_SDO,    ///< I2S output to I2S2 FIFO
    I2S1_TX_FIFO_TO_SDO,    ///< I2S output to I2S1 FIFO
    I2S0_TX_FIFO_TO_SDO,    ///< I2S output to I2S0 FIFO
    ADC_TO_AFE_RX_FIFO,     ///< ADC input to Rx FIFO
    AFE_TX_FIFO_TO_DAC,     ///< DAC output to Tx FIFO
    /*--MCR_V2 MP ONLY*/
    #endif
    MMPF_AUDIO_MAX_FLOW
} MMPF_AUDIO_DATA_FLOW;

//===================//
// Decoder           //
//===================//
#define AMR                 (0x0)       //AMR > 0 can switch to AMR demo mode
#define MP3                 (0x1)
#define AAC                 (0x2)
#define MIDI                (0x3)
#define AACPLUS             (0x4)
#define WMA                 (0x5)
#define WMAPRO              (0x7)
#define OGG                 (0x8)
#define RA                  (0x9)
#define FLAC				(0xA)
#define MP12                (0xB)
#define WAVE                (0x10)

#define AMR_MR475   0
#define AMR_MR515   1

#define AMR_OUTPUT_BUFFER_COUNT 9
#define AAC_OUTPUT_BUFFER_COUNT 6

#define AUDIO_NO_SMOOTH_OP  0x00
#define AUDIO_SMOOTH_UP     0x01
#define AUDIO_SMOOTH_DOWN   0x02

#define A8_AUDIO_STATUS_RECORD  0x8000
#define A8_AUDIO_STATUS_PLAY    0x0000
#define A8_AUDIO_STATUS_START   0x0001
#define A8_AUDIO_STATUS_PAUSE   0x0002
#define A8_AUDIO_STATUS_RESUME  0x0003
#define A8_AUDIO_STATUS_STOP    0x0004

#define MIDI_FILE_MODE      0
#define MIDI_STREAM_MODE    1

#define I2S_DUPLEX              (1)
#define AFE_DUPLEX              (2)

#define PLAY_OP_START           (0x0100)
#define PLAY_OP_STOP            (0x0200)
#define PLAY_OP_PAUSE           (0x0300)
#define PLAY_OP_RESUME          (0x0400)


typedef enum _MMPF_AUDIO_EDIT_POINT
{
    MMPF_AUDIO_EDIT_POINT_ST = 0,
    MMPF_AUDIO_EDIT_POINT_ED
} MMPF_AUDIO_EDIT_POINT;

//Audio Record Event
#define EVENT_FIFO_OVER_THRESHOLD       (0x0002)
#define EVENT_STOP_ENCODER              (0x0004)
#define EVENT_STREAM_OVER_THRESHOLD     (0x0008)
#define EVENT_TRANSCODE_OVER_THRESHOLD  (0x0020)

//Audio Play Event
#define EVENT_DECODE_AACPLUS            (0x0001)
#define EVENT_DECODE_WMA                (0x0002)
#define EVENT_DECODE_WAV                (0x0008)
#define EVENT_DECODE_AMR                (0x0010)
#define EVENT_DECODE_MP3                (0x0020)
#define EVENT_DECODE_AAC                (0x0040)
#define EVENT_DECODE_MIDI               (0x0080)
#define EVENT_DECODE_OGG                (0x0400)
#define EVENT_DECODE_RA             	(0x0800)
#define EVENT_SBC_TRIGGER_INT           (0x1000)
#define EVENT_WAV_TRIGGER_INT           (0x2000)
#define EVENT_SBC_FILLBUF_INT           (0x4000)
#define EVENT_DECODE_AC3                (0x8000)
#define EVENT_DECODE_FLAC				(0x0100)

#define MP3_PLAY_MODE           (0)
#define MIDI_PLAY_MODE          (1)
#define AMR_PLAY_MODE           (2)
#define WMA_PLAY_MODE           (3)
#define AAC_PLAY_MODE           (4)
#define AACPLUS_PLAY_MODE       (7)
#define OGG_PLAY_MODE           (9)
#define VIDEO_AMR_PLAY_MODE     (10)
#define VIDEO_AAC_PLAY_MODE     (11)
#define RA_PLAY_MODE            (12)
#define WAV_PLAY_MODE           (13)
#define VIDEO_MP3_PLAY_MODE     (14)
#define AC3_PLAY_MODE           (15)
#define VIDEO_AC3_PLAY_MODE     (16)
#define VIDEO_RA_PLAY_MODE		(17)
#define VIDEO_WMA_PLAY_MODE     (18)
#define VIDEO_WAV_PLAY_MODE     (19)
#define FLAC_PLAY_MODE			(20)

// refer to MMPD_AUDIO_ENCODE_FMT
#define VIDEO_AMR_REC_MODE      (0)
#define VIDEO_AAC_REC_MODE      (1)
#define AMR_REC_MODE            (2)
#define AAC_REC_MODE            (3)
#define MP3_REC_MODE            (4)
#define WAV_REC_MODE            (5)
#define VIDEO_ADPCM_REC_MODE    (6)
#define VIDEO_MP3_REC_MODE      (7)
#define G711_REC_MODE           (8)
#define VIDEO_PCM_REC_MODE      (9)
#define PCM_REC_MODE            (10)
#define UAC_REC_MODE            (0xE0)
#define AUD_REC_MODE_NONE       (0xFF)

// Number of I2S channels
#define I2S_CH_INVALID          (0xFF)

#define AUDIO_MAX_FILE_SIZE  (256)

#define MIXER_FIFO_WRITE_THRESHOLD    	128
#define MP3_I2S_FIFO_WRITE_THRESHOLD    128
#define WMA_I2S_FIFO_WRITE_THRESHOLD    128
#define OGG_I2S_FIFO_WRITE_THRESHOLD    128
#define	AAC_I2S_FIFO_WRITE_THRESHOLD	128
#define AC3_I2S_FIFO_WRITE_THRESHOLD    128
#define AMR_I2S_FIFO_WRITE_THRESHOLD    80
#define RA_I2S_FIFO_WRITE_THRESHOLD     128
#define WAV_I2S_FIFO_WRITE_THRESHOLD    64
#define FLAC_I2S_FIFO_WRITE_THRESHOLD	128

//=========================================//
//  Audio Play Memory Mode Handshake Buffer//
//=========================================//
#define AUDIO_PLAY_R_HPTR_OFST                  (0)
#define AUDIO_PLAY_R_PTR_OFST                   (4)
#define AUDIO_PLAY_FINISH_SEEK_W                (8)
#define AUDIO_PLAY_START_SEEK_W                 (10)
#define AUDIO_PLAY_FILE_SEEK_OFFSET_L_W         (12)
#define AUDIO_PLAY_FILE_SEEK_OFFSET_H_W         (14)
#define AUDIO_PLAY_W_HPTR_OFST                  (16)
#define AUDIO_PLAY_W_PTR_OFST                   (20)

//===========================================//
//  Audio Record Memory Mode Handshake Buffer//
//===========================================//
#define AUDIO_REC_WRITE_HIGH_PTR_OFFSET_D    (0)
#define AUDIO_REC_WRITE_PTR_OFFSET_D         (4)
#define AUDIO_REC_READ_HIGH_PTR_OFFSET_D     (8)
#define AUDIO_REC_READ_PTR_OFFSET_D          (12)

//===========================================//
//  PMP Project CallBack funtion 			 //
//===========================================//
typedef void AudioPlayerCallBackFunc(void *VidContext, MMP_ULONG flag1, MMP_ULONG flag2);
#if (GAPLESS_EN == 1)
typedef void GaplessNextFileCallBackFunc(MMP_ULONG handle);
#endif
#define         AUDIO_EVENT_EOF     (0x00000001)

typedef struct _AudioCallBackInfo {
	AudioPlayerCallBackFunc *callBackFunction;
	void     *context;
} AudioCallBackInfo;

typedef struct mp4AudioSpecificConfig
{
    /* Audio Specific Info */
    unsigned char objectTypeIndex;
    unsigned char samplingFrequencyIndex;
    unsigned long samplingFrequency;
    unsigned char channelsConfiguration;

    /* GA Specific Info */
    unsigned char frameLengthFlag;
    unsigned short sample256_time;

} mp4AudioSpecificConfig;

typedef struct {
    short rd_index;
    short wr_index;
    unsigned int total_rd;
    unsigned int total_wr;
}AUDIO_DEC_OUTBUF_HANDLE;

typedef struct _MMPF_AUDIO_RINGBUF {
    MMP_ULONG ulBufStart;
    MMP_ULONG ulBufSize;
    MMP_ULONG ulReadPtr;
    MMP_ULONG ulWritePtr;
    MMP_ULONG ulReadPtrHigh;
    MMP_ULONG ulWritePtrHigh;
} MMPF_AUDIO_RINGBUF;

typedef enum {
    MMPF_AUDIO_GAPLESS_SEEK     = 0x0001,
    MMPF_AUDIO_GAPLESS_PAUSE    = 0x0002,
    MMPF_AUDIO_GAPLESS_STOP     = 0x0004,
    MMPF_AUDIO_GAPLESS_OP_ALL   = 0x0007
} AUDIO_GAPLESS_OP;


typedef enum _MMP_AUD_GAIN_CHANGE_MODE{
    MMP_AUD_GAIN_UPDATE_DIRECT,
    MMP_AUD_GAIN_UPDATE_SMOOTH    
}MMP_AUD_GAIN_CHANGE_MODE;

extern void MMPF_PlayMidiDec(void);
extern void MMPF_StopMidiDec(void);
extern void MMPF_PauseMidiDec(void);
extern void MMPF_ResumeMidiDec(void);
extern void MMPF_InitMidiDec(unsigned short mode);
extern void MMPF_WriteMidiStream(unsigned char count);
extern int MMPF_GetMidiFileInfo(unsigned short *total_time);
extern unsigned int MMPF_GetMidiCurTime(void);
MMP_ERR     MMPF_Audio_SetAmrEncodeMode(unsigned short mode);
MMP_ERR     MMPF_Audio_SetStreamLength(unsigned int stream_length);
MMP_ERR     MMPF_SetAudioPlayReadPtr(void);
MMP_ERR     MMPF_Audio_SetupRender(MMP_USHORT usFifoThreshold, MMP_ULONG ulSamplerate);
MMP_ERR     MMPF_Audio_EnableRender(void);
MMP_ERR     MMPF_Audio_DisableRender(void);
MMP_ERR     MMPF_Audio_EnableCapture(MMP_USHORT usFifoThreshold, MMP_ULONG ulSamplerate);
MMP_ERR     MMPF_Audio_DisableCapture(void);
MMP_ERR     MMPF_Audio_InitializeCodec(MMPF_AUDIO_DATA_FLOW path, MMP_ULONG samprate);
MMP_ERR     MMPF_SetAudioRecWritePtr(void);
MMP_ERR     MMPF_SetAudioEncWritePtr(void);
MMP_ERR     MMPF_Audio_SetSpectrumBuf(MMP_ULONG buf_addr);
MMP_ERR     MMPF_UpdateAudioPlayReadPtr(void);
MMP_ERR     MMPF_UpdateAudioPlayWritePtr(void);
MMP_ERR     MMPF_UpdateAudioRecReadPtr(void);
MMP_ERR     MMPF_Audio_SetRecIntThreshold(MMP_USHORT threshold);
MMP_ERR     MMPF_Audio_SetPlayIntThreshold(MMP_USHORT threshold);
MMP_ERR     MMPF_Audio_SetVoiceInPath(MMP_ULONG path);
MMP_ERR     MMPF_Audio_SetVoiceOutPath(MMP_ULONG path);
MMP_BOOL    MMPF_Audio_GetDACPowerOnStatus(void);
MMP_BOOL    MMPF_Audio_GetADCPowerOnStatus(void);
MMP_ERR     MMPF_Audio_PowerOnDAC(MMP_ULONG samplerate);
MMP_ERR     MMPF_Audio_PowerOnADC(MMP_ULONG samplerate);
MMP_ERR     MMPF_Audio_SetPLL(MMP_AUD_INOUT_PATH ubPath, MMP_ULONG ulSamplerate);
MMP_ERR     MMPF_Audio_SetDACFreq(MMP_ULONG samplerate);
MMP_ERR     MMPF_Audio_SetADCFreq(MMP_ULONG samplerate);
MMP_ERR     MMPF_Audio_PowerDownDAC(MMP_BOOL bPowerDownNow);
MMP_ERR     MMPF_Audio_PowerDownADC(void);
MMP_ERR     MMPF_Audio_PowerDownCodec(void);
MMP_ERR     MMPF_PostPlayAudioOp(void);
#if (VMP3_P_EN)
MMP_ERR     MMPF_Audio_UpdateVMP3Time(MMP_ULONG ulTime);
#endif
#if (VAC3_P_EN)
MMP_ERR     MMPF_Audio_UpdateVAC3Time(MMP_ULONG ulTime);
#endif

void MMPF_AUDIO_WaitForSync(void);
MMP_ERR MMPF_AUDIO_GetRecWrPtrEx(MMP_ULONG *ulWrPtr, MMP_ULONG *ulWrWrap, MMP_ULONG *ulRdPtr, MMP_ULONG *ulRdWrap);

MMP_ERR		MMPF_Audio_SetMux(MMPF_AUDIO_DATA_FLOW path, MMP_BOOL bEnable);
MMP_ERR     MMPF_Audio_SetDuplexPath(MMPF_AUDIO_DATA_FLOW path, MMP_BOOL bEnable);
MMP_ERR     MMPF_Audio_SetBypassPath(MMP_UBYTE path);
MMP_ERR  	MMPF_Audio_SetLineInChannel(MMP_AUD_LINEIN_CH lineinchannel);
MMP_ERR     MMPF_Audio_SetPlayFormat(MMP_USHORT mode);
MMP_ERR     MMPF_Audio_GetPlayFormat(MMP_USHORT *decodefmt);

MMP_ERR     MMPF_Audio_SetEncodeFormat(MMP_USHORT mode);
MMP_ERR     MMPF_Audio_SetLiveEncodeFormat( MMP_USHORT  mode,
                                            MMP_ULONG   ulInSamplerate,
                                            MMP_ULONG   ulOutSamplerate,
                                            MMP_UBYTE   ulInCh,
                                            MMP_UBYTE   ulOutCh);
MMP_ERR     MMPF_Audio_SetPlayFileName(MMP_ULONG address);
MMP_ERR  	MMPF_Audio_SetPlayBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize);
MMP_ERR     MMPF_Audio_SetPlayPath(MMP_UBYTE path);
MMP_ERR		MMPF_Audio_SetPlayHandshakeBuf(MMP_ULONG ulBufStart);
MMP_ERR		MMPF_Audio_GetPlayState(MMP_USHORT* usState);
MMP_ERR     MMPF_Audio_GetRecState(MMP_USHORT *usState);
MMP_ERR     MMPF_Audio_GetLiveRecState(MMP_USHORT *usState);
MMP_ERR     MMPF_Audio_SetDACDigitalGain(MMP_UBYTE gain);
MMP_ERR     MMPF_Audio_SetDACAnalogGain(MMP_UBYTE gain);
MMP_ERR  	MMPF_Audio_SetADCDigitalGain(MMP_UBYTE gain, MMP_BOOL bRecordSetting);
MMP_ERR     MMPF_Audio_SetADCAnalogGain(MMP_UBYTE gain, MMP_BOOL boostup, MMP_BOOL bRecordSetting);
MMP_ERR     MMPF_Audio_SetADCMute(MMP_BOOL bMute);
MMP_ERR		MMPF_Audio_InitializePlayFIFO(MMP_USHORT usPath, MMP_USHORT usThreshold);
MMP_ERR 	MMPF_Audio_SetRecordHeadMuteTime(MMP_ULONG ulMilliSec);
MMP_ERR 	MMPF_Audio_SetRecordTailCutTime(MMP_ULONG ulMilliSec);
MMP_ERR 	MMPF_Audio_EnableDummyRecord(MMP_BOOL bEnable, MMP_UBYTE ubFlag);
MMP_ERR     MMPF_Audio_SetRecordSilence(MMP_BOOL bSilence);
MMP_BOOL    MMPF_Audio_IsRecordSilence(void);
MMP_ERR     MMPF_Audio_SetEncodeFormat(MMP_USHORT mode);
MMP_ERR     MMPF_Audio_SetEncodeFileName(MMP_ULONG address);
MMP_ERR  	MMPF_Audio_SetEncodeBuffer(MMP_ULONG ulBufStart, MMP_ULONG ulBufSize);
MMP_ERR     MMPF_Audio_SetEncodePath(MMP_UBYTE path);
MMP_ERR		MMPF_Audio_SetEncodeHandshakeBuf(MMP_ULONG ulBufStart);
MMP_ERR  	MMPF_Audio_SetEncodeLength(MMP_ULONG ulFileLimit);
MMP_ERR		MMPF_Audio_InitializeEncodeFIFO(MMP_USHORT usPath, MMP_USHORT usThreshold);
MMP_ERR 	MMPF_Audio_GetRecordTime(MMP_ULONG *ulMilliSec);

MMP_ERR     MMPF_Audio_SetADCVolume(MMP_SHORT gain, MMP_SHORT* RealDB_INT, MMP_SHORT* RealDB_FRAC);
MMP_ERR     MMPF_AUDIO_UpdateRecWrPtr(MMP_ULONG ulSize);
MMP_ERR     MMPF_AUDIO_SetRecWrPtrWrap(MMP_ULONG ulWrap);
void        MMPF_AUDIO_SetRecWrPtrWrapWoProt(MMP_ULONG ulWrap);
MMP_ERR     MMPF_AUDIO_GetRecWrPtr(MMP_ULONG *ulPtr, MMP_ULONG *ulWrap);

#if (GAPLESS_EN == 1)
MMP_ERR     MMPF_Audio_SetGaplessEnable(MMP_BOOL bEnable, GaplessNextFileCallBackFunc *cb, MMP_ULONG param);
#endif
MMP_ERR     MMPF_Audio_GaplessTriggerCB(void);
MMP_ERR     MMPF_Audio_SetGraphicEQEnable(MMP_BOOL enable);
MMP_ERR     MMPF_Audio_SetGraphicEQBand(MMP_SHORT usFreq, MMP_SHORT usQrange, MMP_SHORT usGain);

MMP_ERR     MMPF_Audio_DummyAudio1CodeStart(void);
MMP_ERR     MMPF_Audio_DummyAudio2CodeStart(void);

#if (AUDIO_PREVIEW_SUPPORT == 1)
MMP_ERR     MMPF_Audio_InitializePreview(MMP_ULONG ulSampleRate);
void        MMPF_Audio_StorePreviewData(MMP_SHORT *sInbuf, MMP_ULONG ulInSamples, MMP_ULONG ulCh);
void        MMPF_Audio_SetPreviewDone(MMP_LONG lTotalSamples);
MMP_BOOL    MMPF_Audio_GetPreviewDone(void);
#endif
#endif
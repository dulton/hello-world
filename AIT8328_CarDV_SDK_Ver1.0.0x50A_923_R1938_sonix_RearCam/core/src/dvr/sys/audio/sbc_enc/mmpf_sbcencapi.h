#ifndef _MMPF_SBCENCAPI_H
#define _MMPF_SBCENCAPI_H

#include "mmp_err.h"
#include "mmpf_audio_ctl.h"


typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;

#define A8_SBC_CHANNEL_MONO     0x00
#define A8_SBC_CHANNEL_DUAL     0x01
#define A8_SBC_CHANNEL_STEREO       0x02
#define A8_SBC_CHANNEL_JOINT        0x03

#define A8_SBC_METHOD_LOUDNESS      0x01
#define A8_SBC_METHOD_SNR       0x02

//-------------------------------------------------

#define SBC_MAX_BLOCKS                  16
#define SBC_MAX_CHANNELS                2
#define SBC_MAX_SUBBANDS                8

#define MAX_SBC_AUDIO_FRAME_SIZE_BYTES  512
#define MAX_SBC_AUDIO_FRAME_SIZE_WORDS  256

#define MAX_SBC_FRAME_SIZE_BYTES        512

typedef enum
{
        SBC_MONO = 0,
        SBC_DUAL,
        SBC_STEREO,
        SBC_JOINT_STEREO
} SbcChannelMode_t;

enum
{
    SBC_FOUR_SUBBANDS        = 0,
    SBC_EIGHT_SUBBANDS
};



typedef unsigned        char    bool_t;


typedef enum
{
        SBC_METHOD_LOUDNESS = 0,
        SBC_METHOD_SNR
} SbcAllocMethod_t;



typedef struct {

    u_int sample_rate;
    int bit_rate;
    int bitpool;
    int nrOfBlocks;
    int ch_mode;
    int allocation;
    int subbands;
    u_char nrOfChannels;
    u_short frameSize;
    u_char  sbcenc_var[1220];// depends on struct sbcenc_frame
} SbcEncCfg_t;



extern MMP_ERR  MMPF_SBCInit(void);
extern MMP_ERR  MMPF_SBCEncodeAudioFrame(short *pSBCInputBufPtr, int InputFrameSize);
extern MMP_ERR  MMPF_SetSBCConfig(u_short parameter, u_short val);
extern MMP_ERR  MMPF_SetSBCOutputBuffer(u_int    bufaddr, u_int bufsize);
extern MMP_ERR  MMPF_GetSBCPacketFrameSize(void);
extern u_int    MMPF_GetSBCEncodeFrameCnt(void);
extern MMP_ERR  MMPF_GetSBCOutputBufTotalIdx(void);
extern MMP_ERR  MMPF_GetSBCOutputBufIdx(void);
extern MMP_ERR  MMPF_SetSBCEncodeEnable(u_char enable,u_int int_period,u_short AudioCodec);
extern u_int    MMPF_SBC_FrameDuration(void);
extern MMP_ERR  MMPF_SBC_CheckBufFullness(void);
#endif


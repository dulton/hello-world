#ifndef AC3API_H
#define AC3API_H

#include "mmpf_audio_ctl.h"
#include "mmpf_audio_typedef.h"

typedef struct {
    unsigned int BitRate;
    unsigned short SampleRate;
    unsigned short HeaderType;
    unsigned short nChannel;
    unsigned int   OutSamples;
    unsigned int	NotSupport;
} AC3FileInfo;

typedef struct {
    unsigned short sample_rate;
    unsigned int sample_cnt;
    unsigned int current_time;
    unsigned int UseSRC;
}AC3_TIME_HANDLE;

#define AC3_BUFDEPTH                3
#define AC3_OUTFRAMESIZE            (256 * 6/*BLK*/ * 2/*CH*/) // A52DEC_FRM_SAMPLES
#define AC3_SIZEOF_WORKING_BUF      (17*1024)
#define AC3DEC_BUFFER_SIZE          (AC3_SIZEOF_WORKING_BUF + \
                                    ((AC3_OUTFRAMESIZE * AC3_BUFDEPTH) << 1))

void InitAc3Decoder(void);
void MMPF_DecodeAc3Frame(void);
void MMPF_PreDecAc3Frame(void);
void MMPF_StartAc3Dec(void);
void MMPF_StopAc3Dec(void);
void MMPF_PauseAc3Dec(void);
void MMPF_ResumeAc3Dec(void);
void MMPF_GetAc3Info(AC3FileInfo* info);
#if 0//SEC_DNSE_VH_ON
void MMPF_AC3Dec_SetVHPar(const int m3deff, const int db, char set_m3deff, char set_db);
#endif

#endif //AC3API_H
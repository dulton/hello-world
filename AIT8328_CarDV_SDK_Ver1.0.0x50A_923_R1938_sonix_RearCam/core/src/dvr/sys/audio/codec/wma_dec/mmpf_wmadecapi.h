

#ifndef MMPF_WMADEC_API_H
#define MMPF_WMADECAPI_H

#include "mmpf_audio_ctl.h"

typedef unsigned int u_int;
typedef unsigned short u_short;
typedef unsigned char u_char;
/*
typedef int Word32;
typedef short Word16;
//typedef char Word8;

typedef unsigned int Word32u;
typedef unsigned short Word16u;
typedef unsigned char Word8u;
*/
#define WMA_OUTFRAMESIZE    6144	//6144 = multiple of 256
#define WMA_OUTFRAMECOUNT   3

#define TAG_SIZE_LIMIT 0x200

extern OutDataObj *pOutData;

extern void MMPF_StartWmaDec(void);
extern void MMPF_StopWmaDec(void);
extern void MMPF_PauseWmaDec(void);
extern void MMPF_ResumeWmaDec(void);


extern void MMPF_GetWmaInfo(void);
void MMPF_SetWmaStartPlayTime(unsigned int start_time);
void MMPF_SetWmaHandShakeMem(unsigned int memory_addr);
void MMPF_SetWmaTagMem(unsigned int memory_addr);

extern int WmaTagInfo;
extern int WMADec_WmaHasVBR;
extern int WmaHasDRM;
typedef struct {
    int title_len;
    int author_len;
	int copyright_len;			
	int description_len;	
	int rating_len;	
} WmaTagLength;
extern WmaTagLength wma_tag_length;
extern u_int MMPF_GetWMADecFramesInByte(void);
extern void MMPF_DecodeWmaFrame(int decoder);
extern void	MMPF_GetWmaSpectrum(void);
#if (AUDIO_MIXER_EN == 0)
extern MMP_ULONG MMPF_GetWMAPlayTime(void);
#endif
#endif

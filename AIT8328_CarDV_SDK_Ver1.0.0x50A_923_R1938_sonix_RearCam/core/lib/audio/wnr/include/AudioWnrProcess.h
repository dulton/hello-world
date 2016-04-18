//==============================================================================
//
//  File        : AudioWnrProcess.h
//  Description : Audio Process function source code
//  Author      : ChenChu Hsu
//  Revision    : 1.0
//
//==============================================================================
#ifndef _AUDIOWNRPROCESS_H_
#define _AUDIOWNRPROCESS_H_

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define CHECK_AUDIO_TIME_USAGE_INLIB_EN (0)

#if CHECK_AUDIO_TIME_USAGE_INLIB_EN
#include "mmpf_typedef.h"
extern volatile unsigned int ulTimer[20];
extern int a;
#endif

typedef enum {
	WNR_EQ_OFF					= 0,
	WNR_EQ_DEFAULT		 		= -1,
	WNR_EQ_USER_DEFINE          = 1
} WNR_EQ_MODE;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef struct {
	unsigned int point_number; // can only be 256
	unsigned int channel;
	unsigned int beamforming_en;
	unsigned int fs;
	unsigned int fc;
	unsigned int nf_degree;
} AudioWnrProcessStruct;

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
int FuncAudioWnrProcess_Init(char * workingBufferAddress, AudioWnrProcessStruct * wnr_process_struct);
int FuncAudioWnrProcess_DoOneFrame(short* pssAduioIn);
unsigned int FuncAudioWnrProcess_SetIIREnable(unsigned int enable);
unsigned int FuncAudioWnrProcess_SetNFEnable(unsigned int enable);
//unsigned int FuncAudioWnrProcess_GetWorkingBufLength(void);
unsigned int FuncAudioWnrProcess_SetWNREnable(unsigned int enable);
unsigned int FuncAudioWnrProcess_GetLibVersion(unsigned short* ver_year, unsigned short* ver_date, unsigned short* ver_time);
void FuncAudioWnrProcess_SetEQ(WNR_EQ_MODE mode, short *weight_x256);
#endif // #ifndef _AUDIOWNRPROCESS_H_

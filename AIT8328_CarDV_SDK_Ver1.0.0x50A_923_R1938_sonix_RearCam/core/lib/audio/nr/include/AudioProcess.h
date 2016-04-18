//==============================================================================
//
//  File        : AudioProcess.h
//  Description : Audio Process function source code
//  Author      : ChenChu Hsu
//  Revision    : 1.0
//
//==============================================================================
#ifndef _AUDIOPROCESS_H_
#define _AUDIOPROCESS_H_

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define CHECK_AUDIO_TIME_USAGE_INLIB_EN (0)
#define eqCOSTDOWN_EN					(1)
#define COSTDOWN_EN						(0)
#define DUMP_NR_INFO_EN					(0)

#if DUMP_NR_INFO_EN
#include <stdio.h>
FILE *file_filt;
FILE *file_spec;
//FILE *file_fact;
#endif

#if CHECK_AUDIO_TIME_USAGE_INLIB_EN
#include "mmpf_typedef.h"
extern volatile unsigned int ulTimer[20];
extern int a;
#endif

typedef enum {
	AP_DR_IN_FROM_INSIDE				= 0,
	AP_DR_IN_FROM_OUTSIDE 				= 1
} AP_DR_IN_MODE;

typedef enum {
	NO_VAD					= 0,
	VAD_STAGE_1 			= 1,
	VAD_STAGE_2             = 2,
	VAD_STAGE_3				= 3
} VAD_MODE;

typedef enum {
	EQ_OFF					= 0,
	EQ_DEFAULT		 		= -1,
	EQ_USER_DEFINE          = 1
} EQ_MODE;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef struct {
	//unsigned int noise_floor;
	unsigned int point_number; // can only be 256
	unsigned int channel;
	int beamforming_en;  // only useful when channel == 2
} AudioProcessStruct;

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
int FuncAudioProcess_Init(char * workingBufferAddress, AudioProcessStruct * audio_process_struct);
int FuncAudioProcess_DoOneFrame(short* pssAduioIn);
// unsigned int FuncAudioProcess_GetWorkingBufLength(void);
unsigned int FuncAudioProcess_SetNREnable(unsigned int enable);
unsigned int FuncAudioProcess_SetNRSmoothLevel(unsigned int level);
unsigned int FuncAudioProcess_SetNRCostDownLevel(unsigned int level);
unsigned int FuncAudioProcess_SetNRMode(int mode);
unsigned int FuncAudioProcess_SetDeReverberationEnable(unsigned int enable);
unsigned int FuncAudioProcess_SetDeReverberationInBufMode(AP_DR_IN_MODE in_mode);
int* FuncAudioProcess_GetDeReverberationInBufAddress(void);
int FuncAudioProcess_GetDeReverberationInHalfBufSize(void);
int FuncAudioProcess_GetSpeechProb(void);
int FuncAudioProcess_Get_VadOut (void); // there are 2 stages
void FuncAudioProcess_Set_Vad_Mode(VAD_MODE mode); // NO_VAD : no VAD
												   // VAD_STAGE_$ : VAD stage number $
void FuncAudioProcess_Set_Vad_Threshold(int level);
void FuncAudioProcess_Set_EQ(EQ_MODE mode,short* weight_x256); //mode : EQ_DEFAULT for default setting HPF with cutoff frequency 400Hz
																		//       EQ_OFF with no EQ effect
																		//       EQ_USER_DEFINE use user-defined weight_x256[257] for EQ

unsigned int FuncAudioProcess_GetLibVersion(unsigned short* ver_year, unsigned short* ver_date, unsigned short* ver_time);
#endif // #ifndef _AUDIOPROCESS_H_

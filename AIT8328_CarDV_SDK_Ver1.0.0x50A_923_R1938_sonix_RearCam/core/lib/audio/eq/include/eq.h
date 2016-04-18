#ifndef	eq_h
#define	eq_h

#define EQ_NONE			0
#define EQ_CLASSIC		1
#define EQ_JAZZ			2
#define EQ_POP			3
#define EQ_ROCK			4
#define EQ_BASS3		5
#define EQ_BASS9		6
#define LOUDNESS        7
#define EQ_SPK          8
#define VIRTUAL_BASS    9

#define COEFF_B_LEN		4
#define COEFF_A_LEN		2

//wilson@110826: define for virtual bass eq
/* Speaker Fc Settings (Hz) */ 
#define BASS_LARGE_SPKFC       ((int)(40)) 
#define BASS_MIDDLE1_SPKFC     ((int)(90)) 
#define BASS_MIDDLE2_SPKFC     ((int)(120)) 
#define BASS_SMALL_SPKFC       ((int)(150)) 

//For "Gain": Linear gain for bass 
/* Bass Gain Step (Q7)*/ 
#define BASS_GAIN_VERYLARGE    ((int)(2.5*0x7F)) 
#define BASS_GAIN_LARGE        ((int)(2.0*0x7F)) 
#define BASS_GAIN_MIDDLE       ((int)(1.5*0x7F)) 
#define BASS_GAIN_SMALL        ((int)(1.0*0x7F)) 
#define BASS_GAIN_NONE         ((int)(0.0*0x7F)) 

//For "HPFEnable": To enable input high pass filter or not 
/* Input Highpass Filter */ 
#define BASSE_HPF_ENABLE       (1) 
#define BASSE_HPF_DISABLE      (0)

typedef struct {
	MMP_ULONG SampleRate;    /* Hz */ 
	MMP_ULONG nCh;           /* Number of channel */ 
	MMP_ULONG Fc;            /* Bass management cut-off frequency */ 
	MMP_ULONG Gain; /* Bass gain */ 
	MMP_ULONG HPFEnable;
}VirtualBassEQHandler;
//#define SFTNUM			2
// Scale down factor for EQ input, Q.15 format
#define SCALE_DOWN_2dB      26028
#define SCALE_DOWN_2_5dB    24572
#define SCALE_DOWN_3dB      23197
#define SCALE_DOWN_4dB      20676
#define SCALE_DOWN_5dB      18426
#define SCALE_DOWN_5_5dB    17395
#define SCALE_DOWN_6dB      16384
#define SCALE_DOWN_8dB      13041
#define SCALE_DOWN_8_5dB    12315
#define SCALE_DOWN_9dB      11626
#define SCALE_DOWN_10dB     10362
#define SCALE_DOWN_11dB     9235

typedef struct {
	short   	*pEQCoef;
	short		memb[4];	//0 -1 -2 -3
	short		mema[2];	//-1 -2
	short		enable;		//enable flag
	short       scale_factor;
} EQStruct;

int InitEQ_VOL(EQStruct *pEQ, char kind, short volume);
void A810L_ClearEQBuf(void);
extern int EQFilter2s(EQStruct *pEQ, short *buf, int len);
extern void BassE_Init(int SampleRate,    /* Hz */ 
                int nCh,           /* Number of channel */ 
                int Fc,            /* Bass management cut-off frequency */ 
                unsigned int Gain, /* Bass gain */ 
                int HPFEnable);    /* Enable High-pass filter for input */ 
extern int BassEnhance(short *pInput,  /* Input buffer address */ 
                short *pOutput, /* Output buffer address */ 
                int DecFrameSz  /* Number of samples of each channel for batch process */ 
               ); 
#endif
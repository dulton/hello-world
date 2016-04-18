#ifndef _SRC_RESAMPLE_H_
#define	_SRC_RESAMPLE_H_
/*
 * FILE: SRC_resample.h
 */


#define	USE_DOUBLE_FACTOR


//#define IBUFFSIZE 4096                        /* Input buffer size */
#define SRC_IBUFFSIZE (128+20)                      /* Input buffer size */
//#define SRC_IBUFFSIZE (64+20)                      /* Input buffer size */


#define	MAX_SRATE_TYPE	9

#define	SRATE_48000		48000
#define	SRATE_44100		44100
#define	SRATE_32000		32000
#define SRATE_24000		24000
#define SRATE_22050		22050
#define SRATE_16000		16000
#define SRATE_12000		12000
#define SRATE_11025		11025
#define SRATE_8000		8000



typedef char           BOOL;
typedef short          HWORD;
typedef unsigned short UHWORD;
typedef int            WORD;
typedef unsigned int   UWORD;
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define MAX_HWORD (32767)
#define MIN_HWORD (-32768)



typedef struct RESAMPLE_PARE
{
    int insrate;
#ifdef USE_DOUBLE_FACTOR
    int outsrate;
	double factor;              /* factor = Sndout/Sndin */
#else
    int outsrate_int;
	int factor_x2;             	/* factor = Sndout/Sndin */
#endif
    int inCount;                /* number of input samples to convert */
    int outCount;               /* number of output samples to compute */

    BOOL linearInterp;		/* 0 = highest quality, 1 = slowest speed */

	//----------------------------
	// In FrameResampleFast
	//----------------------------
    UWORD	Time;          /* Current time/pos in input sample */
    UWORD	Time2;          /* Current time/pos in input sample */
    UHWORD Xp;
    UHWORD Xoff;
    UHWORD Xread;
//    HWORD	Y1[IBUFFSIZE*6]; /* I/O buffers */
    UHWORD Nx;
    int		Ycount;
    int		last;
}Resample_Para_t;


#define	SRC_EXTENDED_IIR_BUFFER_SIZE    16
#define	SRC_FILTER_ORDER1_LPF           10

typedef struct SRC_MUL_INS
{
	
	int 	SRC_InBufleftsize;
	short	*SRC_InBufleft;
	short	*SRC_TempInBufL;
	short	*SRC_TempInBufR;
	short	*SRC_TempOutBufL;	
	short	*SRC_TempOutBufR;
	int   SRC_index1;	
	int   SRC_x_buf_0[(SRC_FILTER_ORDER1_LPF+SRC_EXTENDED_IIR_BUFFER_SIZE)*2];
	int   SRC_y_buf_0[(SRC_FILTER_ORDER1_LPF+SRC_EXTENDED_IIR_BUFFER_SIZE)*2]; 	
	Resample_Para_t SRC_Info;

}SRC_MUL_INS;


extern	int SRC_SetWorkingBuf(SRC_MUL_INS *s, int *buf);
#ifdef USE_DOUBLE_FACTOR
//extern	int SRC_Config(Resample_Para_t *prsamp, int insrate, int newsrate);
extern	int SRC_Config(SRC_MUL_INS *prsamp, int insrate, int newsrate);

#else
//extern	int SRC_Config(Resample_Para_t *prsamp, int insrate, int newsrate_int);
extern	int SRC_Config(SRC_MUL_INS *prsamp, int insrate, int newsrate_int);

#endif
extern	UHWORD SRC_FrameResampleFast(  /* number of output samples returned */
	SRC_MUL_INS *s,
	//Resample_Para_t *prsamp,
    short 	*inbufsL,			//input buffer
    short 	*inbufsR,			//input buffer
    short 	*obufsL,			//output buffer
    short 	*obufsR,			//output buffer
    int		nChans
);

extern	void SRC_iir_filter_stereo_asm(short *inBuf, short *outBuf, int SamplePerChannel);
extern	void SRC_iir_filter_mono(short *inBuf, short *outBuf, int SamplePerChannel);
extern	short SRC_FrameResample(SRC_MUL_INS *s, short *inbuf, short *outbuf, int inbufsize, int nChans);



    
#endif //end #ifndef _SRC_RESAMPLE_H_

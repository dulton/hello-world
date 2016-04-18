#ifndef _MMPF_SRCAPI_H_
#define	_MMPF_SRCAPI_H_
/*
 * FILE: mmpf_srcapi.h
 */

#define SRC_USE_DOUBLE_FACTOR

//SRC_IBUFFSIZE could not be changed.It is fixed inside SRC_Lib.a
#define SRC_IBUFFSIZE (128+20)    /* Input buffer size *///
#define	SRC_SIZEOF_WORKING_BUF  (4736)      // SRC_IBUFFSIZE*16*2
#define	SRC_INBUF_SIZE  512                 // Input buffer size to SRC procedure.
                                            // For example, 2304 samples = 512+512+512+512+256
#define	SRC_OUTBUF_SIZE (SRC_INBUF_SIZE*7)	// Output buffer size after SRC procedure.
                                            // max 8k to 48k = 6 times upsample and one temp buffer left.

#define	MAX_SRATE_TYPE	9

#define	SRATE_192000    192000
#define	SRATE_128000    128000
#define	SRATE_96000     96000
#define	SRATE_64000     64000
#define	SRATE_48000		48000
#define	SRATE_44100		44100
#define	SRATE_32000		32000
#define SRATE_24000		24000
#define SRATE_22050		22050
#define SRATE_16000		16000
#define SRATE_12000		12000
#define SRATE_11025		11025
#define SRATE_8000		8000



//typedef char           BOOL;
//typedef short          HWORD;
//typedef unsigned short UHWORD;
//typedef int            WORD;
//typedef unsigned int   UWORD;

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

//#define MAX_HWORD (32767)
//#define MIN_HWORD (-32768)


#define SRC_OUTPUT_RATIO 7
#define SRC_OUTPUT_DEPTH 5
#define SRC_READ_SAMPLE     (256)

typedef struct RESAMPLE_PARE
{
    int insrate;
#ifdef SRC_USE_DOUBLE_FACTOR
    int outsrate;
	double factor;              /* factor = Sndout/Sndin */
#else
    int outsrate_int;
	int factor_x2;             	/* factor = Sndout/Sndin */
#endif
    int inCount;                /* number of input samples to convert */
    int outCount;               /* number of output samples to compute */

    char linearInterp;		/* 0 = highest quality, 1 = slowest speed */

	//----------------------------
	// In FrameResampleFast
	//----------------------------
    unsigned int	Time;          /* Current time/pos in input sample */
    unsigned int	Time2;          /* Current time/pos in input sample */
    unsigned short Xp;
    unsigned short Xoff;
    unsigned short Xread;
//    short	Y1[IBUFFSIZE*6]; /* I/O buffers */
    unsigned short Nx;
    int		Ycount;
    int		last;
}Resample_Para_t;

/*---- multi-instance ----*/
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
#ifdef SRC_USE_DOUBLE_FACTOR
extern	int SRC_Config(SRC_MUL_INS *prsamp, int insrate, int newsrate);
#else
extern	int SRC_Config(SRC_MUL_INS *prsamp, int insrate, int newsrate_int);
#endif
extern short SRC_FrameResample(SRC_MUL_INS *s, short *inbuf, short *outbuf, int inbufsize, int outbufsize, int nChans);
extern	int SRC_err_msg(char *s);

    
#endif //end #ifndef _MMPF_SRCAPI_H_
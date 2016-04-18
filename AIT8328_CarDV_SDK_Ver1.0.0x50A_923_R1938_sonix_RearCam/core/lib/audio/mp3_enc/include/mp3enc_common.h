
#ifndef COMMON_DOT_H
#define COMMON_DOT_H


/***********************************************************************
*
*  Global Include Files
*
***********************************************************************/

//#include        <stdio.h>
//#include        <string.h>
//#include        <math.h>

#include				"mp3enc_tables.h"

typedef short  MP3Enc_buffer_type[2][1152];


#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/***********************************************************************
*
*  Global Definitions
*
***********************************************************************/

/* General Definitions */
#ifndef FALSE
#define         FALSE                   0
#endif

#ifndef TRUE
#define         TRUE                    1
#endif

#define         NULL_CHAR               '\0'

#define         MAX_U_32_NUM            0xFFFFFFFF
#ifndef PI
#define         PI                      3.14159265358979
#endif
#define         PI4                     PI/4
#define         PI64                    PI/64
#define         LN_TO_LOG10             0.2302585093

#define         VOL_REF_NUM             0
#define         MPEG_AUDIO_ID           1
#define		    MPEG_PHASE2_LSF		    0	/* 1995-07-11 SHN */

//#define         MONO                    1
//#define         STEREO                  2
#define         BITS_IN_A_BYTE          8
//#define         WORD                    16
#define         MAX_NAME_SIZE           81
#define         SBLIMIT                 32
#define         SSLIMIT                 18
#define         FFT_SIZE                1024
#define         HAN_SIZE                512
#define         SCALE_BLOCK             12
#define         SCALE_RANGE             64
#define         SCALE                   32768
#define         CRC16_POLYNOMIAL        0x8005

/* MPEG Header Definitions - Mode Values */

#define         MPG_MD_STEREO           0
#define         MPG_MD_DUAL_CHANNEL     2
#define         MPG_MD_MONO             3

/* Mode Extention */

#define         MPG_MD_LR_LR             0
#define         MPG_MD_LR_I              1
#define         MPG_MD_MS_LR             2
#define         MPG_MD_MS_I              3


/* "bit_stream.h" Definitions */

#define         MINIMUM         4    /* Minimum size of the buffer in bytes */
#define         MAX_LENGTH      32   /* Maximum length of word written or
                                        read from bit stream */
#define         READ_MODE       0

#define         WRITE_MODE      1
#define         ALIGNING        8

#define        LOG2             0.69314718055994530942
//#define        LOG2             0.30102999

#define         BUFFER_SIZE     4096

#ifndef			MIN
#define         MIN(A, B)       ((A) < (B) ? (A) : (B))
#endif

#ifndef 		MAX
#define         MAX(A, B)       ((A) > (B) ? (A) : (B))
#endif

#define MAX_32 (int)0x7fffffffL
#define MIN_32 (int)0x80000000L

#define MAX_16 (short)0x7fff
#define MIN_16 (short)0x8000

#define NORM_TYPE       0
#define START_TYPE      1
#define SHORT_TYPE      2
#define STOP_TYPE       3

/***********************************************************************
*
*  Global Type Definitions
*
***********************************************************************/

/* Structure for Reading Layer II Allocation Tables from File */

typedef struct {
    unsigned int    steps;
    unsigned int    bits;
    unsigned int    group;
    unsigned int    quant;
} sb_alloc, *alloc_ptr;

typedef sb_alloc        al_table[SBLIMIT][16];

/* Header Information Structure */

typedef struct {
    int version;
    int error_protection;
    int bitrate_index;
    int sampling_frequency;
    int padding;
    int extension;
    int mode;
    int mode_ext;
    int copyright;
    int original;
    int emphasis;
/*		int	freq; */
} layer, *the_layer;

/* Parent Structure Interpreting some Frame Parameters in Header */

typedef struct {
    layer       *header;        /* raw header information */
    int         actual_mode;    /* when writing IS, may forget if 0 chs */
    al_table    *alloc;         /* bit allocation table read in */
    int         tab_num;        /* number of table as loaded */
    int         stereo;         /* 1 for mono, 2 for stereo */
    int         jsbound;        /* first band of joint stereo coding */
    int         sblimit;        /* total number of sub bands */
} frame_params;



enum byte_order { order_unknown, order_bigEndian, order_littleEndian };
extern enum byte_order NativeByteOrder;

/* "bit_stream.h" Type Definitions */

typedef struct  bit_stream_struc {
//    FILE        *pt;            /* pointer to bit stream device */
    unsigned char *buf;         /* bit stream buffer */
    int         buf_size;       /* size of buffer (in number of bytes) */
    int			totbit;         /* bit counter of bit stream */

    int         buf_byte_idx;   /* pointer to top byte in buffer */
    int         buf_bit_idx;    /* pointer to top bit of top byte in buffer */
    int         mode;           /* bit stream open in read or write mode */
    int         eob;            /* end of buffer index */
    int         eobs;           /* end of bit stream flag */
    char        format;
    
    /* format of file in rd mode (BINARY/ASCII) */
} Bit_stream_struc;

#include "mp3enc_l3side.h"

/***********************************************************************
*
*  Global Variable External Declarations
*
***********************************************************************/
extern double   s_freq[2][4];
extern int      bitratex[2][15];

/***********************************************************************
*
*  Global Function Prototype Declarations
*
***********************************************************************/

/* The following functions are in the file "common.c" */
extern void         *mem_alloc(unsigned int block);
//extern void         mem_free( void *ptr_addr);
extern void 		hdr_to_frps(frame_params *fr_ps); /* interpret data in hdr str to fields in fr_ps */
extern int          f_div(int x, int y, int frac_bits);
extern int          div_s_arm(int var1, int var2);
extern int          FFracSqrt(int x); 
extern void         Log2 (
                        int      L_x,         /* (i) : input value                                 */
                        int      *exponent,   /* (o) : Integer part of Log2.   (range: 0<=val<=30) */
                        int      *fraction    /* (o) : Fractional part of Log2. (range: 0<=val<1) */
                    );



/* log/log10 approximations */
#define    FAST_LOG(x)      log(x)


#define    OFFSET           (int)(15*LOG2*(1<<16)) 
#define    OFFSET1          (int)(31*LOG2*(1<<16)) 
#define    LOG2_Q15         FRAC_CONST_16(LOG2)
#define    TBB_CNST1        FRAC_CONST_16(-0.299)
#define    TBB_CNST2        FRAC_CONST_16(0.43)
#define    SNR_CNST1        23*Q26_PRECISION               // 23.0 in (6.26) format    
#define    SNR_CNST2        6 *Q26_PRECISION               // 6.0 in (6.26) format   
#define    EXP_DELTA        (int)(((29.0 - 6.0)/255.0)*Q26_PRECISION)

#define    TANTWELFTHPI     tan(PI/12)
#define    TANSIXTHPI       tan(PI/6) 
#define    SIXTHPI          PI/6

#define    HALFPI           COEF_CONST(PI/2)  
#define    ONEPI            COEF_CONST(PI)       
#define    THIRD2PI         COEF_CONST(3*PI/2)  
#define    TWOPI            COEF_CONST(2*PI)  
#define    POSITIVE         0
#define    NEGATIVE         1  

#define     INT_BITS        32
//#define     MANT_DIGITS     9
#define     MANT_DIGITS     10
#define     MANT_SIZE       (1<<MANT_DIGITS)


#define Q26_BITS 26          
#define Q26_PRECISION (unsigned int)((1<<Q26_BITS) - 1)
#define Q3_BITS 3            
#define Q3_PRECISION (unsigned int)((1<<Q3_BITS) - 1)
#define Q4_BITS 4            
#define Q4_PRECISION (unsigned int)((1<<Q4_BITS) - 1)
#define Q10_BITS 10            
#define Q10_PRECISION_16 (unsigned int)((1<<Q10_BITS) - 1)
#define COEF_BITS 28        // change from 28 to 25(Q.25 format) to prevent (phi1 - phi_prime) overflow
#define COEF_PRECISION (1 << COEF_BITS)
#define REAL_BITS 14 // MAXIMUM OF 14 FOR FIXED POINT SBR
#define REAL_PRECISION (1 << REAL_BITS)

/* FRAC is the fractional only part of the fixed point number [0.0..1.0) */
#define FRAC_SIZE 32 /* frac is a 32 bit integer */
#define FRAC_BITS 31
#define FRAC_PRECISION ((unsigned int)((1 << FRAC_BITS)) - 1)
#define FRAC_MAX 0x7FFFFFFF

#define FRAC_SIZE_16 16 /* frac is a 16 bit integer */
#define FRAC_BITS_16 15
#define FRAC_PRECISION_16 ((unsigned int)((1 << FRAC_BITS_16)-1))
#define FRAC_MAX_16 0x7FFF

// For fractional division(Q format adjustable)
#define F_FRACBITS		24                      //  (8.24) format
#define f_intpart(x)	((x) >> F_FRACBITS)
#define F_MIN		    ((int) -0x80000000L)
#define F_MAX		    ((int) +0x7fffffffL)
#define F_PRECISION     (unsigned int)((1<<F_FRACBITS) - 1)    


#define REAL_CONST(A) ((int)((A)*(REAL_PRECISION)+0)) 
#define COEF_CONST(A) ((int)((A)*(COEF_PRECISION)+0)) 
#define Q26_CONST(A)  ((int)((A)*(Q26_PRECISION)+0)) 
#define Q4_CONST(A)   ((int)((A)*(Q4_PRECISION)+0))
#define Q10_CONST_16(A)  ((short)((A)*(Q10_PRECISION_16)+0)) 
#define FRAC_CONST(A) (((A) == 1.00) ? ((int)FRAC_MAX) : (((A) >= 0) ? ((int)((A)*(FRAC_PRECISION)+0)) : ((int)((A)*(FRAC_PRECISION)-0))))
#define FRAC_CONST_16(A) (((A) == 1.00) ? ((short)FRAC_MAX_16) : (((A) >= 0) ? ((short)((A)*(FRAC_PRECISION_16)+0)) : ((short)((A)*(FRAC_PRECISION_16)-0))))
#define FRAC_CONST_16x16(A) ((short)((A*16)*(FRAC_PRECISION_16)+0)) 
#define FRAC_CONST_16x8(A) ((short)((A*8)*(FRAC_PRECISION_16)+0)) 
#define FRAC_CONST_16x2(A) ((short)((A*2)*(FRAC_PRECISION_16)+0)) 
#define FRAC_CONST_x8(A) ((int)((A*8)*(FRAC_PRECISION)+0)) 

#define MUL_F(A,B) (int)(((__int64)(A)*(__int64)(B)) >> FRAC_BITS)
#define MUL_F32(A,B) (int)(((__int64)(A)*(__int64)(B)) >> FRAC_BITS_16)
#define MUL_F16(A,B) (int)(((int)(A)*(int)(B)) >> FRAC_BITS_16)
#define MUL_F_DIV8(A,B) (int)(((__int64)(A)*(__int64)(B)) >> (FRAC_BITS+3))

#endif



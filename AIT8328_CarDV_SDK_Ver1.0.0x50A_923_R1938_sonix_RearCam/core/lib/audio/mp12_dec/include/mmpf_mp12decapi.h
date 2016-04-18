#ifndef MP12DEC_API
#define MP12DEC_API

#define MP12_MAX_CHANNELS    2
#define MP12_SBLIMIT         32 /* number of subbands */

typedef struct _AIT_MPEGAud12DecContext {
    int bit_rate;
    int layer;
    int sample_rate;    //Hz
    int channels;       //number of audio channels
    int frame_size;
    int frame_number;   //audio frame number
    void *priv_data;
    int parse_only; 
} AIT_MPEGAud12DecContext;

extern int AIT_MPA12DecS_Init(AIT_MPEGAud12DecContext *avctx, void *workingbuf, int workingbuf_sz);
extern int AIT_MPA12DecS_DecFrame(AIT_MPEGAud12DecContext *avctx,
                                    void *data, int *data_size,
                                    unsigned char *inBuf, int inBuf_size);
#endif //MP12DEC_API

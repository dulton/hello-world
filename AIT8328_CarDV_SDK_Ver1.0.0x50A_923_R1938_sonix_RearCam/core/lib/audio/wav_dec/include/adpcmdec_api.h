#ifndef _ADPCMDEC_API_H_
#define _ADPCMDEC_API_H_


typedef struct AIT_AdpcmDecCtx 
{
    int AdpcmDCtxBitRate;
    int AdpcmDCtxSampleRate; 
    int AdpcmDCtxnCh;    
    int AdpcmDCtxSampleFmt;
    int AdpcmDCtxCID;
    int AdpcmDCtxBlkAlign;
    int AdpcmDCtxFrmSz;
    int AdpcmDCtxBitPerEncSample;

    unsigned char *AdpcmDCtxExtraData;
    int AdpcmDCtxExtraDataSz;

    void *AdpcmDCtxPrivData;

} AIT_AdpcmDecCtx;


extern int AIT_ADPCMDecS_Init(  AIT_AdpcmDecCtx * AdpcmD_Ctx, 
                                unsigned int * working_buffer, 
                                unsigned int working_buffer_size);

extern int AIT_ADPCMDecS_DecFrame(  AIT_AdpcmDecCtx *AdpcmD_Ctx,
                                    void *data, 
                                    int *data_size,
                                    void *InBufAddr, 
                                    int InBufSz);

#endif //_ADPCMDEC_API_H_



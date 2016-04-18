
#ifndef __WMVDEC_INTERFACE_H_
#define __WMVDEC_INTERFACE_H_
#define DECODETORAWFILE       // we decode all audio+video streams to raw files

// Do not parse or decode audio if audio decoder codec is not present
#ifdef WMF_EXCLUDE_AUDIO
#define AUDIOPATTERN    Discard_WMC         // No audio decoder: ignore audio stream
#else
#define AUDIOPATTERN    Decompressed_WMC    // Audio decoder present: decode audio strm
#endif

typedef struct _MMPF_WMVDEC_CONTEXT {
    unsigned int ulCompression;
    unsigned int ulCompressionOut;
    unsigned int ulWMVWorkingBufStart;
    unsigned int ulInitBSStart;
    unsigned int ulInitBSSize;
    unsigned int ulBitrate;
    unsigned int ulWidth;
    unsigned int ulHeight;
} MMPF_WMVDEC_CONTEXT;


#define PCM_SAMPLE_SIZE  4096                                       // size of audio pcm buffer (samples per channel)
#define AUDIOBUFFERSIZE  (9 * PCM_SAMPLE_SIZE * sizeof(I16_WMC))    // actual audio buffer size


void wmvdec_setworkingbuf(int *ptr);
void wmvdec_setvidbsbuf(unsigned char *ptr);
MMP_ULONG MMPF_WMVDEC_GetTempBSAddr(void);
MMP_ERR MMPF_WMVDEC_GetFrame(MMP_UBYTE *dest_addr, MMP_ULONG BufSize1, MMP_ULONG BufSize2, MMP_ULONG ulFrameSize);
void wmvdec_setaudbsbuf(unsigned char *ptr);
void MMPF_WMVDEC_GetAudioInfo(MMP_ULONG *SR, MMP_ULONG *BR, MMP_ULONG *CH);
void MMPF_WMVDEC_InitAudioBSBuf(MMP_USHORT usIndex);
void wmvdec_fprintf(MMP_ULONG level, char *str, int val);
MMP_ERR MMPF_WMVPSR_GetNextSample(MMPF_WMVPSR_MEDIA_TYPE MediaType, MMP_WMV_SAMPLE_INFO *pSample);
MMP_ERR MMPF_WMVDEC_WMVSeekNextKeyFrame(MMP_SHORT sAdvance, MMP_ULONG *RequestFrame, MMP_WMV_SAMPLE_INFO *pSample);
MMP_ULONG MMPF_WMVDEC_WMVGetFrameNum(MMP_BYTE Index);
MMP_ERR MMPF_WMVDEC_WMVSeekTime(MMP_ULONG ulTime, MMP_ULONG* ulReturnTime,MMPF_WMVPSR_MEDIA_TYPE MediaType, MMP_BOOL bNextKey, MMP_BOOL bNearest, MMP_BOOL bSeekAudioOnly);
MMP_ERR MMPF_WMVDEC_GetEncodeOpt(MMP_USHORT *usOpt);
void MMPF_WMVDEC_GetFileInfo(MMP_VIDEO_CONTAINER_INFO *info);
int wmvdec_init(MMP_VIDEO_CONTAINER_INFO *info);
void wmvdec_uninit(void);
MMP_USHORT MMPF_WMVDEC_GetFormatTag(void);

#if (AIT_WMDRM_SUPPORT == 1)
MMP_BOOL MMPF_WMVDEC_GetWMDRMEnable(void);
#endif

extern	void *wmvdec_malloc(unsigned int size);
extern  void *wmvdec_realloc(void *ptr, unsigned int size);
extern  void wmvdec_fprintf(MMP_ULONG level, char *str, int val);
extern  void  PrintDebugMsg(char *s);
#endif

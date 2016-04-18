#ifndef _FLACDECAPI_H_
#define _FLACDECAPI_H_

#include    "includes_fw.h"
#include    "lib_retina.h"

#if 0 //samyu: oggflac
#define FLAC_INBUF_SIZE      	(1024*4)
#else //samyu: oggflac
#define FLAC_INBUF_SIZE      	(1024*8)
#endif
#define FLAC_SIZEOF_WORKING_BUF ((4096+9228+4608)*4) 
#define FLAC_MAX_FRAME_SIZE		4608
#define FLAC_OUTFRAME_SIZE    	(FLAC_MAX_FRAME_SIZE<<1)	//(8192)
#define FLAC_OUTFRAME_COUNT		3
#define FLAC_BS_SIZE 			4096
#define FLACDEC_BUFFER_SIZE     (FLAC_SIZEOF_WORKING_BUF + \
                                FLAC_INBUF_SIZE + \
                                ((FLAC_OUTFRAME_SIZE * FLAC_OUTFRAME_COUNT) << 1) + \
                                (FLAC_OUTFRAME_SIZE << 1))

typedef struct 
{
    unsigned int BitRate;
    unsigned int TotalTime;	  //ms
    unsigned short SampleRate;
    unsigned short nChannel;

    unsigned __int64 TotalSamples;
    unsigned short BitsPerSample;	  //bits per sample

} FLACFileInfo;


extern	MMP_ERR MMPF_StartFlacDec(void);
extern	void MMPF_PauseFlacDec(void);
extern	void MMPF_ResumeFlacDec(void);
extern	void MMPF_StopFlacDec(void);
extern	void MMPF_GetFlacInfo(FLACFileInfo* info);
extern	void MMPF_SetFLACStartPlayTime(MMP_ULONG ulMilliSec);
extern	MMP_ULONG MMPF_GetFLACPlayTime(void);
extern	void MMPF_DecodeFlacFrame(int decoder);
#if (AUDIO_PREVIEW_SUPPORT == 1)
extern  MMP_ERR MMPF_StartFlacPreview(void);
extern  MMP_ERR MMPF_DecodeFlacForPreview(void);
#endif

#endif  //end of #ifndef _FLACDECAPI_H_


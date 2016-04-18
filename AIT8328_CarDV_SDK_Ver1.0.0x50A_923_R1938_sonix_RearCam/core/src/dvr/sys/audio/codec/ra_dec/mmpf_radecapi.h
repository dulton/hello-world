#ifndef _MMPF_RADECAPI_H_
#define _MMPF_RADECAPI_H_

#include "includes_fw.h"
#include "lib_retina.h"

#define	RA_PCM_OUTBUF_SIZE	(2048*2) //Update by CY 2010/11/30, (2240)
#define	RA_WORKING_BUF_SIZE	(64*1024 + 19944 + 424) //Update by CY 2010/11/30, (64*1024 + 19944)	// in bytes
//19944 = sizeof(struct _DecBufs); in Gecko2Info.
#define	RADEC_SIZEOF_WORKING_BUF	(RA_WORKING_BUF_SIZE + (RA_PCM_OUTBUF_SIZE << 1))
#define	RA_OUTFRAMESIZE		(2048*8)	// At least 2048*8
#define RA_OUTFRAMEUNIT     (2048)
#define RA_OUTFRAMECOUNT    (8)
#define RADEC_BUFFER_SIZE   (RADEC_SIZEOF_WORKING_BUF + \
                            (RA_OUTFRAMESIZE << 1) + \
                            (RA_PCM_OUTBUF_SIZE << 1))

typedef struct {
    unsigned int sample_rate;
    unsigned int sample_left;
    unsigned int sample_cnt;
    unsigned int current_time;
    unsigned int target_time;
    unsigned int NeedDecode;
    unsigned int CheckTime;
    unsigned int TimeOutCnt;
    unsigned int previous_time;
} RA_TIME_HANDLE;

void MMPF_StartRADec(void);
void MMPF_StopRADec(void);
void MMPF_PauseRADec(void);
void MMPF_ResumeRADec(void);
void MMPF_GetRAInfo(void);
void MMPF_SetRAStartPlayTime(int start_time);
MMP_ULONG MMPF_GetRADecCurrentPlayTime(void);
void MMPF_DecodeRAFrame(int decoder);
MMP_ERR MMPF_PreDecRAFrame(int decoder);
void MMPF_SetRAHeader(void* addr);
#if (AUDIO_PREVIEW_SUPPORT == 1)
void MMPF_SetRAPreviewing(MMP_BOOL bEnable);
MMP_ERR MMPF_StartRAPreview(void);
MMP_ERR MMPF_DecodeRAForPreview(void);
#endif
#endif

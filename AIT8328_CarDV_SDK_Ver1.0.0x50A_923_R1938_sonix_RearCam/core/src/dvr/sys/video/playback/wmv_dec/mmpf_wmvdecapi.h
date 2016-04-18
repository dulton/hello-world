#ifndef _MMP_WMVPARSER_H_
#define _MMP_WMVPARSER_H_

#include "mmpf_viddec_def.h"

#define WMV_INTERNAL_BSSIZE (128*1024)

typedef enum MMPF_WMVPSR_MEDIA_TYPE{
	WMV_AUDIO_TYPE,                               
	WMV_VIDEO_TYPE                                   
}MMPF_WMVPSR_MEDIA_TYPE;

typedef struct MMP_WMV_SAMPLE_INFO
{
    MMP_ULONG ulOffset;    ///< The offset in the file of the sample
    MMP_ULONG ulSize;      ///< The size of the sample
    MMP_LONG lSampleTime; ///< The tick of the sample
}MMP_WMV_SAMPLE_INFO;


MMP_HANDLE MMPF_WMVDEC_Open(MMPF_VIDEO_FUNC *func);
#endif
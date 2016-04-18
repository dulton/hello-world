

#ifndef MMPF_WMAPRODECAPI_H
#define MMPF_WMAPRODECAPI_H

typedef unsigned int u_int;			
typedef unsigned short u_short;				
typedef unsigned char u_char; 

typedef int Word32;
typedef short Word16;
//typedef char Word8;

typedef unsigned int Word32u;
typedef unsigned short Word16u;
typedef unsigned char Word8u;

//WMAPro external malloc: WMAPro Lib needs at least 90KB for working buffer
#define ALLOCATE_WMAPRO_RAM_SIZE        (30*1024)
#define ALLOCATE_WMAPRO_SRAM_SIZE       (60*1024)
#define WMAPRO_RAM_SIZE_NEEDED          (ALLOCATE_WMAPRO_RAM_SIZE + ALLOCATE_WMAPRO_SRAM_SIZE)  
#define WMAPRODEC_SIZEOF_WORKING_BUF	(90*1024)

#define WMAPRO_OUTFRAMESIZE	    8192 
#define WMAPRO_OUTFRAMECOUNT    2   //at least 2
#define WMAPRODEC_BUFFER_SIZE   (WMAPRODEC_SIZEOF_WORKING_BUF + \
                                ((WMAPRO_OUTFRAMESIZE * WMAPRO_OUTFRAMECOUNT) << 1) +\
                                (WMAPRO_OUTFRAMESIZE << 1))

#define TAG_SIZE_LIMIT 0x200

extern OutDataObj *pOutData;

extern void MMPF_PreDecWmaProDec(void);

extern void MMPF_StartWmaProDec(void);
extern void MMPF_StopWmaProDec(void);
extern void MMPF_PauseWmaProDec(void);
extern void MMPF_ResumeWmaProDec(void);
extern void MMPF_GetWmaProInfo(void);
void MMPF_SetWmaProStartPlayTime(int start_time); //use signed int
void MMPF_SetWmaProTagMem(unsigned int memory_addr);
extern u_int MMPF_GetWMAProDecFramesInByte(void);
extern u_int MMPF_GetWMAProPlayTime(void);

extern void MMPF_DecodeWmaProFrame(int decoder);
extern void	MMPF_GetWmaProSpectrum(void);

extern int WMAProDec_TagInfo;
extern int WMAProDec_HasVBR;
extern int WMAProDec_HasDRM;

extern unsigned int WMAProDec_TotalTime;
    
typedef struct {
	int	title_len;				
	int	author_len;				
	int copyright_len;			
	int description_len;	
	int rating_len;	
} WmaTagLength;

// for Video + WMA
 
#if (VWMA_P_EN)
#define WMV_TIME_BUFFER_NUM (10)
#define WMV_AVSYNC_SAMPLE_NUM (4096)

typedef struct _MMPF_WMVDEC_TIME_HANDLE{
    MMP_ULONG ulTime[WMV_TIME_BUFFER_NUM];
    MMP_ULONG ulCurrentTime;
    MMP_ULONG ulRdSampleCnt;
    MMP_USHORT usRdTimeIdx;
    MMP_USHORT usWrTimeIdx;
    MMP_USHORT usSampleRate;
    MMP_USHORT usOldSampleRate;
    MMP_USHORT usWrSampleCnt;
    MMP_ULONG  ulWriteTime;
    MMP_ULONG  ulReadTime;
} MMPF_WMVDEC_TIME_HANDLE;


typedef struct _WMAFormat {
    MMP_USHORT wFormatTag;
    MMP_USHORT nChannels;
    MMP_ULONG nSamplesPerSec;
    MMP_ULONG nAvgBytesPerSec;
    MMP_USHORT nBlockAlign;
    MMP_USHORT nValidBitsPerSample;
    MMP_ULONG nChannelMask;
    MMP_USHORT wEncodeOpt;
    MMP_USHORT wAdvancedEncodeOpt;
    MMP_ULONG dwAdvancedEncodeOpt2;
} WMAFormat;

extern void MMPF_WMADEC_GetWMAFormat(WMAFormat **pWMAFormat);

#endif

// End for Video + WMA
#if (AUDIO_PREVIEW_SUPPORT == 1)
MMP_ERR MMPF_StartWMAPreview(void);
MMP_ERR MMPF_DecodeWMAForPreview(void);
#endif

extern WmaTagLength wmapro_tag_length;

#endif //MMPF_WMAPRODECAPI_H

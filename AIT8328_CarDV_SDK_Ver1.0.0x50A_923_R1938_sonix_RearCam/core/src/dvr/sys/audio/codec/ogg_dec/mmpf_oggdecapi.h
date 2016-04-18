

#ifndef MMPF_OGGDEC_API_H
#define MMPF_OGGDEC_API_H

#include "mmpf_audio_typedef.h"

typedef unsigned int u_int;			
typedef unsigned short u_short;				
typedef unsigned char u_char; 

//typedef int Word32;
//typedef short Word16;
//typedef char Word8;

//typedef unsigned int Word32u;
//typedef unsigned short Word16u;
//typedef unsigned char Word8u;

#define	OGGDEC_SIZEOF_WORKING_BUF	            (52*1024)
#define	OGGDEC_SIZEOF_WORKING_BUF_NONCACHEABLE	(16*1024)

#define OGG_OUTFRAMESIZE	        8192//2048	
#define OGG_OUTFRAMECOUNT           3
#define OGGDEC_BUFFER_SIZE          (OGGDEC_SIZEOF_WORKING_BUF + \
                                    OGGDEC_SIZEOF_WORKING_BUF_NONCACHEABLE + \
                                    ((OGG_OUTFRAMESIZE * OGG_OUTFRAMECOUNT) << 1) + \
                                    (OGG_OUTFRAMESIZE << 1))

#define TAG_SIZE_LIMIT 0x200

extern OutDataObj *pOutData;

extern MMP_ERR MMPF_StartOggDec(void);
extern void MMPF_StopOggDec(void);
extern void MMPF_PauseOggDec(void);
extern void MMPF_ResumeOggDec(void);


extern void MMPF_GetOggInfo(void);
void MMPF_SetOggStartPlayTime(unsigned int start_time);
unsigned int MMPF_GetOggPlayTime(void);
void MMPF_SetOggHandShakeMem(unsigned int memory_addr);
void MMPF_SetOggTagMem(unsigned int memory_addr);

extern int OggTagInfo;
extern int OggHasDRM;
typedef struct {
	int	title_len;	
	int version_len;				
	int	album_len;		
	int	artist_len;					
	int copyright_len;			
	int description_len;		
} OggTagLength;
extern OggTagLength ogg_tag_length;
extern void MMPF_DecodeOggFrame(int decoder);
extern void	MMPF_GetOggSpectrum(void);
#if (AUDIO_PREVIEW_SUPPORT == 1)
extern MMP_ERR MMPF_StartOggPreview(void);
extern MMP_ERR MMPF_DecodeOggForPreview(void);
#endif
#endif

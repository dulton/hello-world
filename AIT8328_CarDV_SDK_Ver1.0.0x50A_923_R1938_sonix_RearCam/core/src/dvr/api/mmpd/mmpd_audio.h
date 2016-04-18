/// @ait_only
//==============================================================================
//
//  File        : mmpd_audio.h
//  Description : Audio Control Driver Function
//  Author      : Hnas Liu
//  Revision    : 1.0
//
//==============================================================================



#ifndef _MMPD_AUDIO_H_
#define _MMPD_AUDIO_H_

#include "ait_config.h"
#include "mmp_aud_inc.h"

/** @addtogroup MMPD_AUDIO
 *  @{
 */

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define MMPD_AUDIO_PLAY_HANDSHAKE_BUF_SIZE          (24)

#define MMPD_AUDIO_PLAY_R_HPTR_OFST                 (0)
#define MMPD_AUDIO_PLAY_R_PTR_OFST                  (4)
#define MMPD_AUDIO_PLAY_FINISH_SEEK_W               (8)
#define MMPD_AUDIO_PLAY_START_SEEK_W                (10)
#define MMPD_AUDIO_PLAY_FILE_SEEK_OFFSET_L_W        (12)
#define MMPD_AUDIO_PLAY_FILE_SEEK_OFFSET_H_W        (14)
#define MMPD_AUDIO_PLAY_W_HPTR_OFST                 (16)
#define MMPD_AUDIO_PLAY_W_PTR_OFST                  (20)

#define MMPD_AUDIO_REC_HANDSHAKE_BUF_SIZE           (8)
#define MMPD_AUDIO_REC_WRITE_HIGH_PTR_OFFSET_W      (0)
#define MMPD_AUDIO_REC_WRITE_PTR_OFFSET_W           (2)
#define MMPD_AUDIO_REC_READ_HIGH_PTR_OFFSET_W       (4)
#define MMPD_AUDIO_REC_READ_PTR_OFFSET_W            (6)

typedef enum _MMPD_AUDIO_PLAY_DATAPATH
{
    MMPD_AUDIO_PLAY_FROM_MEM = 0,   ///< memory mode
    MMPD_AUDIO_PLAY_FROM_CARD,      ///< card mode
    MMPD_AUDIO_PLAY_FROM_MMP_MEM    ///< MMP memory mode
} MMPD_AUDIO_PLAY_DATAPATH;

typedef enum _MMPD_AUDIO_RECORD_DATAPATH
{
    MMPD_AUDIO_RECORD_TO_MEM = 0,///< memory mode
    MMPD_AUDIO_RECORD_TO_CARD    ///< card mode
} MMPD_AUDIO_RECORD_DATAPATH;

#define MMPD_AUDIO_SPECTRUM_DATA_SIZE  (64)

#define MMPD_AUDIO_MAX_VOLUME_LEVEL    (39)

#define MMPD_AUDIO_MAX_MIC_VOLUME_LEVEL   (10)

#define OGG_TAG_BUF_SIZE    (512)

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPD_AUDIO_AACPLUS_INFO {
    MMP_ULONG   ulBitRate;      // bit rate
    MMP_USHORT  usSampleRate;   // sampling rate
    MMP_USHORT  usHeaderType;   // header type:0->unknown, 1->ADIF, 2->ADTS
    MMP_USHORT  usChans;        // channel num
    MMP_ULONG	ulTotalTime;    // file total time(unit: sec)
    MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
} MMPD_AUDIO_AACPLUS_INFO;

typedef struct _MMPD_AUDIO_FLAC_INFO {
    MMP_ULONG   ulBitRate;      // bit rate
    MMP_ULONG64   ulTotalSamples; // total samples
    MMP_USHORT  usSampleRate;   // sampling rate
    MMP_USHORT  usChans;        // channel num
    MMP_ULONG	ulTotalTime;    // file total time(unit: sec)
    MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
} MMPD_AUDIO_FLAC_FILE_INFO;

typedef struct _MMPD_AUDIO_MIDI_INFO {
        MMP_USHORT  usSampleRate;   //sampleing rate
        MMP_ULONG   ulTotalTime;    //total time(unit: milli-sec)
        MMP_USHORT  usNotSupport;   // 0 : supported format; 1 : not supported format
} MMPD_AUDIO_MIDI_FILE_INFO;

typedef struct _MMPD_AUDIO_MP3_INFO {
        MMP_ULONG 	ulBitRate;      //unit: bps
        MMP_USHORT  usSampleRate;   //unit: Hz
        MMP_USHORT  usChans;        //1->mono, 2->stereo
        MMP_USHORT  usLayer;        //3->layer 3
        MMP_USHORT  usVersion;      //0->MPEG1, 1->MPEG2, 2->MPEG2.5
        MMP_ULONG	ulTotalTime;    //unit: milli-sec
		MMP_USHORT	usVbr;          //0:CBR, 1:VBR
        MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
        MMP_BOOL    bIsMp3HdFile;   //MMP_TRUE: mp3HD file
} MMPD_AUDIO_MP3_INFO;

typedef struct _MMPD_AUDIO_AMR_INFO {
        MMP_ULONG   ulBitRate;      // bit rate
        MMP_USHORT	usMode;         //0:4.75k, 7:12.2k
        MMP_USHORT	usSampleSize;   //frame size
        MMP_USHORT  usSampleRate;   //sample rate
        MMP_ULONG	ulTotalTime;    //unit: milli-sec
        MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
} MMPD_AUDIO_AMR_INFO;

typedef struct _MMPD_AUDIO_WMA_INFO {
        MMP_ULONG   ulBitRate;      //bit rate
        MMP_ULONG   ulSampleRate;   //sampling rate
        MMP_USHORT  usChans;        //channel num
        MMP_USHORT  usBitsPerSample;//bit per sample
        MMP_ULONG	ulTotalTime;    //unit: milli-sec
        MMP_USHORT  usTagInfo;      //0:no tag, 1:tag exist
        MMP_USHORT  usVbrInfo;      //0:CBR, 1:VBR
        MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
        MMP_USHORT  usVersion;      //0, 1, 2: WMA Std. bitstream,  3: WMA Pro bitstream
} MMPD_AUDIO_WMA_INFO;

typedef struct _MMPD_AUDIO_RA_INFO {
        MMP_ULONG   ulBitRate;      //bit rate
        MMP_USHORT  usSampleRate;   //sampling rate
        MMP_USHORT  usChans;        //channel num
        MMP_ULONG	ulTotalTime;    //unit: milli-sec
        MMP_USHORT  usVbrInfo;      //0:CBR, 1:VBR
        MMP_USHORT  usNotSupport;   //0: supported format; 1: not supported format
        MMP_BOOL    bSeekable;      //0: non-seekable; 1: seekable
} MMPD_AUDIO_RA_INFO;

typedef struct _MMPD_AUDIO_OGG_INFO {
        MMP_ULONG   ulBitRate;      //bit rate
        MMP_USHORT  usSampleRate;   //sampling rate
        MMP_USHORT  usChans;        //channel num
        MMP_ULONG	ulTotalTime;    //unit: milli-sec
        MMP_USHORT  usTagInfo;      //0:no tag, 1:tag exist
        MMP_USHORT  usVbrInfo;      //0:CBR, 1:VBR
        MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
} MMPD_AUDIO_OGG_INFO;

typedef struct _MMPD_AUDIO_WAV_INFO {
        MMP_ULONG   ulBitRate;      //bit rate
        MMP_ULONG   ulTotalTime;    //unit: milli-sec
        MMP_ULONG   ulSampleRate;   //sampling rate
        MMP_ULONG   ulNewSampleRate;//new sampling rate
        MMP_USHORT  usChans;        //channel num
        MMP_USHORT  usNotSupport;   //0 : supported format; 1 : not supported format
} MMPD_AUDIO_WAV_INFO;


typedef struct {
	    MMP_ULONG	title_len;	
	    MMP_ULONG   version_len;				
	    MMP_ULONG	album_len;		
	    MMP_ULONG	artist_len;					
	    MMP_ULONG   copyright_len;			
	    MMP_ULONG   description_len;		
} MMPD_AUDIO_OGG_TAG_INFO;

typedef struct {
        MMP_ULONG   ulBitRate;      // bit rate
        MMP_USHORT  usSampleRate;   // sampling rate
        MMP_USHORT  usHeaderType;   // header type:0->unknown, 1->ADIF, 2->ADTS
        MMP_USHORT  usChans;        // channel num
        MMP_ULONG	ulTotalTime;    // file total time(unit: sec)
        MMP_USHORT  usNotSupport;   // 0 : supported format; 1 : not supported format
} MMPD_AUDIO_AAC_FILE_INFO;

/* Wind noise reduction configuration */
typedef struct _MMPD_AUDIO_WNR_CFG {
    MMP_BOOL    bEnWNR;          ///< Noise filter enable
    MMP_BOOL    bEnNR;          ///< Noise reduction enable
} MMPD_AUDIO_WNR_CFG;

//==============================================================================
//
//                              ENUMERATIONS
//
//==============================================================================

typedef enum _MMPD_AUDIO_I2S_FORMAT
{
    MMPD_AUDIO_I2S_MASTER = 0,      ///< master mode
    MMPD_AUDIO_I2S_SLAVE            ///< slave mode
} MMPD_AUDIO_I2S_FORMAT;

typedef enum _MMPD_AUDIO_EXTCODEC_PATH
{
    MMPD_AUDIO_EXT_SPK_OUT = 0,     ///< speaker output path
    MMPD_AUDIO_EXT_HP_OUT,          ///< headphone output path
    MMPD_AUDIO_EXT_MIC_IN,          ///< mic in path
    MMPD_AUDIO_EXT_AUX_IN,          ///< AUX in path
    MMPD_AUDIO_EXT_MIC_BYPASS_SPK,  ///< mic in bypass to speaker out
    MMPD_AUDIO_EXT_MIC_BYPASS_HP,   ///< mic in bypass to headphone out
    MMPD_AUDIO_EXT_AUX_BYPASS_SPK,  ///< AUX in bypass to speaker out
    MMPD_AUDIO_EXT_AUX_BYPASS_HP,   ///< AUX in bypass to headphone out
    MMPD_AUDIO_EXT_MIC_IN_SPK_OUT,  ///< mic in and speaker out
    MMPD_AUDIO_EXT_MIC_IN_HP_OUT,   ///< mic in and headphone out
    MMPD_AUDIO_EXT_AUX_IN_SPK_OUT,  ///< AUX in and speaker out
    MMPD_AUDIO_EXT_AUX_IN_HP_OUT,   ///< AUX in and headphone out
    MMPD_AUDIO_EXT_PATH_MAX
} MMPD_AUDIO_EXTCODEC_PATH;

typedef enum _MMPD_AUDIO_ENCODE_FMT
{
    MMPD_AUDIO_VAMR_ENCODE = 0,     ///< video + amr encode mode
    MMPD_AUDIO_VAAC_ENCODE = 1,     ///< video + aac encode mode
    MMPD_AUDIO_AMR_ENCODE = 2,      ///< pure amr encode mode
    MMPD_AUDIO_AAC_ENCODE = 3,      ///< pure aac encode mode
    MMPD_AUDIO_MP3_ENCODE = 4,      ///< pure mp3 encode mode
    MMPD_AUDIO_WAV_ENCODE = 5,      ///< pure wav encode mode
    MMPD_AUDIO_VADPCM_ENCODE = 6,   ///< video + adpcm encode mode
    MMPD_AUDIO_VMP3_ENCODE = 7,     ///< video + mp3 encode mode
    MMPD_AUDIO_G711_ENCODE = 8,     ///< G711 mu law encode mode
    MMPD_AUDIO_VPCM_ENCODE = 9,     ///< video + raw PCM encode mode
    MMPD_AUDIO_PCM_ENCODE = 10      ///< pure pcm mode
} MMPD_AUDIO_ENCODE_FMT;

typedef enum _MMPD_AUDIO_DECODE_FMT
{
    MMPD_AUDIO_MP3_PLAY = 0,		///< mp3 play mmode
    MMPD_AUDIO_MIDI_PLAY = 1,		///< midi play mode
    MMPD_AUDIO_AMR_PLAY = 2,		///< amr play mode
    MMPD_AUDIO_WMA_PLAY = 3,		///< wma play mode
    MMPD_AUDIO_AAC_PLAY = 4,		///< aac play mode
    MMPD_AUDIO_PCM_PLAY = 5,		///< pcm play mode
    MMPD_AUDIO_AACPLUS_PLAY = 7,	///< aac+ play mode
	MMPD_AUDIO_OGG_PLAY = 9,		///< ogg play mode
    MMPD_AUDIO_VAMR_PLAY = 10,		///< video + amr play mode
    MMPD_AUDIO_VAAC_PLAY = 11,		///< video + aac play mode
	MMPD_AUDIO_RA_PLAY = 12,        ///< ra play mode
	MMPD_AUDIO_WAV_PLAY = 13,       ///< wav play mode
	MMPD_AUDIO_VMP3_PLAY = 14,       ///< video + mp3 play mode
    MMPD_AUDIO_AC3_PLAY = 15,		///< ac3 play mmode	
	MMPD_AUDIO_VAC3_PLAY = 16,       ///< video + ac3 play mode	    
	MMPD_AUDIO_VRA_PLAY = 17,       ///< video + ra play mode	    
	MMPD_AUDIO_VWMA_PLAY = 18,
	MMPD_AUDIO_VWAV_PLAY = 19,
	MMPD_AUDIO_FLAC_PLAY = 20
} MMPD_AUDIO_DECODE_FMT;


#if (GAPLESS_PLAY_EN == 1)
typedef enum _MMPD_AUDIO_GAPLESS_OP
{
    MMPD_AUDIO_GAPLESS_SEEK     = 0x0001,
    MMPD_AUDIO_GAPLESS_PAUSE    = 0x0002,
    MMPD_AUDIO_GAPLESS_STOP     = 0x0004,
    MMPD_AUDIO_GAPLESS_OP_ALL   = 0x0007
} MMPD_AUDIO_GAPLESS_OP;
#endif

typedef enum _MMPD_AUDIO_I2S_WORK_MODE {
    MMPD_AUDIO_I2S_WORK_MODE_NONE = 0,
    MMPD_AUDIO_I2S_MASTER_MODE,             ///< BCK & LRCK is outputed by AIT
    MMPD_AUDIO_I2S_SLAVE_MODE,              ///< BCK & LRCK is outputed by external codec
    MMPD_AUDIO_I2S_MAX_WORK_MODE
} MMPD_AUDIO_I2S_WORK_MODE;

typedef enum _MMPD_AUDIO_I2S_MCLK_MODE {
    MMPD_AUDIO_I2S_MCLK_MODE_NONE = 0,
    MMPD_AUDIO_I2S_128FS_MODE,              ///< 128*fs clock output
    MMPD_AUDIO_I2S_192FS_MODE,              ///< 192*fs clock output
    MMPD_AUDIO_I2S_256FS_MODE,              ///< 256*fs clock output
    MMPD_AUDIO_I2S_384FS_MODE,              ///< 384*fs clock output
    MMPD_AUDIO_I2S_512FS_MODE,              ///< 512*fs clock output
    MMPD_AUDIO_I2S_768FS_MODE,              ///< 768*fs clock output
    MMPD_AUDIO_I2S_1024FS_MODE,             ///< 1024*fs clock output
    MMPD_AUDIO_I2S_1536FS_MODE,             ///< 1536*fs clock output
    MMPD_AUDIO_I2S_FIX_256FS_MODE,          ///< fixed 256*fs clock output
    MMPD_AUDIO_I2S_USB_MODE,                ///< fixed clock output
    MMPD_AUDIO_I2S_MAX_MCLK_MODE
} MMPD_AUDIO_I2S_MCLK_MODE;

typedef enum _MMPD_AUDIO_I2S_OUT_BITS {
    MMPD_AUDIO_I2S_OUT_BITS_NONE = 0,
    MMPD_AUDIO_I2S_OUT_16BIT,               ///< 16 bits for each channel
    MMPD_AUDIO_I2S_OUT_24BIT,               ///< 24 bits for each channel
    MMPD_AUDIO_I2S_OUT_32BIT,               ///< 32 bits for each channel
    MMPD_AUDIO_I2S_MAX_OUT_BIT_MODE
} MMPD_AUDIO_I2S_OUT_BITS;

typedef enum _MMPD_AUDIO_I2S_IN_BITS {
    MMPD_AUDIO_I2S_IN_BITS_NONE = 0,
    MMPD_AUDIO_I2S_IN_16BIT,               ///< 16 bits for each channel
    MMPD_AUDIO_I2S_IN_20BIT,               ///< 20 bits for each channel
    MMPD_AUDIO_I2S_IN_24BIT,               ///< 24 bits for each channel
    MMPD_AUDIO_I2S_MAX_IN_BIT_MODE
} MMPD_AUDIO_I2S_IN_BITS;

typedef enum _MMPD_AUDIO_I2S_WS_POLARITY {  ///< word select polarity
    MMPD_AUDIO_I2S_LRCK_POLARITY_NONE = 0,
    MMPD_AUDIO_I2S_LRCK_L_LOW,              ///< L channel is low, R channel is high
    MMPD_AUDIO_I2S_LRCK_L_HIGH,             ///< L channel is high, R channel is low
    MMPD_AUDIO_I2S_MAX_WS_POLARITY
} MMPD_AUDIO_I2S_WS_POLARITY;

typedef enum _MMPD_AUDIO_I2S_DELAY_MODE {
    MMPD_AUDIO_I2S_OUT_DELAY_NONE = 0,
    MMPD_AUDIO_I2S_OUT_DELAY_I2S,           ///< I2S mode output data delay
    MMPD_AUDIO_I2S_OUT_DELAY_STD,           ///< standard mode output data delay
    MMPD_AUDIO_I2S_MAX_OUT_DELAY_MODE
} MMPD_AUDIO_I2S_DELAY_MODE;

//==============================================================================
//
//                              FUNCTION PROTOTYPE
//
//==============================================================================
extern MMP_ERR	MMPD_AUDIO_PlayFadeOutEn(MMP_UBYTE channelID, MMP_BOOL bAFadeOutEn);

extern MMP_ERR  MMPD_AUDIO_GetDecOutBuf(MMP_ULONG *ulBufAddr, MMP_ULONG *ulBufSize);
extern MMP_ERR	MMPD_AUDIO_CheckPlayEnd(void);
extern MMP_ERR  MMPD_AUDIO_GetAACPlusFileInfo(MMPD_AUDIO_AACPLUS_INFO *aacplusinfo);
extern MMP_ERR  MMPD_AUDIO_GetMIDIFileInfo(MMPD_AUDIO_MIDI_FILE_INFO *midiinfo);
extern MMP_ERR  MMPD_AUDIO_GetMP3FileInfo(MMPD_AUDIO_MP3_INFO *mp3info);
extern MMP_ERR  MMPD_AUDIO_GetWMAFileInfo(MMPD_AUDIO_WMA_INFO *wmainfo);
extern MMP_ERR  MMPD_AUDIO_GetRAFileInfo(MMPD_AUDIO_RA_INFO *rainfo);
extern MMP_ERR  MMPD_AUDIO_GetAMRFileInfo(MMPD_AUDIO_AMR_INFO *amrinfo);
extern MMP_ERR  MMPD_AUDIO_GetAACFileInfo(MMPD_AUDIO_AAC_FILE_INFO *aacinfo);
extern MMP_ERR  MMPD_AUDIO_GetWAVFileInfo(MMPD_AUDIO_WAV_INFO *wavinfo);
extern MMP_ERR	MMPD_AUDIO_GetFLACFileInfo(MMPD_AUDIO_FLAC_FILE_INFO *flacinfo);
extern MMP_ERR  MMPD_AUDIO_ReadAACPlusFileInfo(MMPD_AUDIO_AACPLUS_INFO *aacplusinfo);
extern MMP_ERR  MMPD_AUDIO_ReadMIDIFileInfo(MMPD_AUDIO_MIDI_FILE_INFO *midiinfo);
extern MMP_ERR  MMPD_AUDIO_ReadMP3FileInfo(MMPD_AUDIO_MP3_INFO *mp3info);
extern MMP_ERR  MMPD_AUDIO_ReadWMAFileInfo(MMPD_AUDIO_WMA_INFO *wmainfo);
extern MMP_ERR  MMPD_AUDIO_ReadAMRFileInfo(MMPD_AUDIO_AMR_INFO *amrinfo);
extern MMP_ERR  MMPD_AUDIO_ReadOGGFileInfo(MMPD_AUDIO_OGG_INFO *ogginfo);
extern MMP_ERR  MMPD_AUDIO_ReadRAFileInfo(MMPD_AUDIO_RA_INFO *rainfo);
extern MMP_ERR  MMPD_AUDIO_ReadWAVFileInfo(MMPD_AUDIO_WAV_INFO *wavinfo);
extern MMP_ERR	MMPD_AUDIO_ReadFLACFileInfo(MMPD_AUDIO_FLAC_FILE_INFO *flacinfo);
extern MMP_ERR  MMPD_AUDIO_StopAudioPlay(void);
extern MMP_ERR	MMPD_AUDIO_ResumeAudioPlay(void);
extern MMP_ERR	MMPD_AUDIO_PauseAudioPlay(void);
extern MMP_ERR	MMPD_AUDIO_StartAudioPlay(void* , void* );
extern MMP_ERR  MMPD_AUDIO_GetAACPlusPlayTime(MMP_USHORT *usPosRatio,MMP_ULONG *ulMilliSecond);
extern MMP_ERR	MMPD_AUDIO_GetMIDIPlayTime(MMP_USHORT *usTimeRatio,MMP_ULONG *ulMilliSecond);
extern MMP_ERR  MMPD_AUDIO_GetMP3PlayTime(MMP_USHORT *usPosRatio,MMP_ULONG *ulMilliSecond);
extern MMP_ERR  MMPD_AUDIO_GetWMAPlayTime(MMP_USHORT *usPosRatio,MMP_ULONG *ulMilliSecond);
extern MMP_ERR  MMPD_AUDIO_GetRAPlayTime(MMP_USHORT *usPosRatio,MMP_ULONG *ulMilliSecond);
extern MMP_ERR  MMPD_AUDIO_GetOGGPlayTime(MMP_ULONG *ulMilliSecond);
extern MMP_ERR  MMPD_AUDIO_GetWAVPlayTime(MMP_ULONG *ulMilliSecond);
extern MMP_ERR  MMPD_AUDIO_GetAMRPlayTime(MMP_USHORT *usPosRatio,MMP_ULONG *ulMilliSecond);
extern MMP_ERR	MMPD_AUDIO_GetFLACPlayTime(MMP_USHORT *usPosRatio, MMP_ULONG *ulMilliSecond);
extern MMP_ERR	MMPD_AUDIO_SetMIDIPlayTime(MMP_ULONG ulTime);
extern MMP_ERR	MMPD_AUDIO_SetAACPlusPlayTime(MMP_ULONG ulMilliSec);
extern MMP_ERR	MMPD_AUDIO_SetMP3PlayTime(MMP_ULONG ulMilliSec);
extern MMP_ERR	MMPD_AUDIO_SetWMAPlayTime(MMP_ULONG ulTime);
extern MMP_ERR	MMPD_AUDIO_GetWMAVersion(MMP_ULONG *ulVersion);
extern MMP_ERR	MMPD_AUDIO_SetRAPlayTime(MMP_ULONG ulTime);
extern MMP_ERR	MMPD_AUDIO_SetOGGPlayTime(MMP_ULONG ulTime);
extern MMP_ERR  MMPD_AUDIO_SetWAVPlayTime(MMP_ULONG ulMilliSec);
extern MMP_ERR	MMPD_AUDIO_SetAACPlayTime(MMP_ULONG ulMilliSec);
extern MMP_ERR	MMPD_AUDIO_SetAMRPlayTime(MMP_ULONG ulMilliSec);
extern MMP_ERR	MMPD_AUDIO_SetFLACPlayTime(MMP_ULONG ulMilliSec);
extern MMP_ERR  MMPD_AUDIO_GetPlayStatus(MMP_USHORT *usStatus);
extern MMP_ERR	MMPD_AUDIO_SetEncodeFileSize(MMP_ULONG ulSize);
extern MMP_ERR	MMPD_AUDIO_StartAudioRecord(void);
extern MMP_ERR	MMPD_AUDIO_PauseAudioRecord(void);
extern MMP_ERR	MMPD_AUDIO_ResumeAudioRecord(void);
extern MMP_ERR	MMPD_AUDIO_StopAudioRecord(void);
extern MMP_ERR	MMPD_AUDIO_StartLiveAudioRecord(void);
extern MMP_ERR 	MMPD_AUDIO_PauseLiveAudioRecord(void);
extern MMP_ERR 	MMPD_AUDIO_ResumeLiveAudioRecord(void);
extern MMP_ERR 	MMPD_AUDIO_StopLiveAudioRecord(void);

extern MMP_ERR	MMPD_AUDIO_SetPlayPosition(MMP_ULONG ulPos);
extern MMP_ERR	MMPD_AUDIO_EnableABRepeatMode(MMP_USHORT usEnable);
extern MMP_ERR	MMPD_AUDIO_SetPlayVolume(MMP_UBYTE ubSWGain, MMP_UBYTE ubDGain, MMP_UBYTE ubAGain, MMP_BOOL bSWGainOnly);
extern MMP_ERR	MMPD_AUDIO_SetRecordVolume(MMP_UBYTE ubDGain, MMP_UBYTE ubAGain, MMP_BOOL bboostup);
extern MMP_ERR	MMPD_AUDIO_SetRecordHeadMuteTime(MMP_ULONG ulMilliSec);
extern MMP_ERR 	MMPD_AUDIO_SetRecordTailCutTime(MMP_ULONG ulMilliSec);
extern MMP_ERR	MMPD_AUDIO_StopMIDINotePlay(void);
extern MMP_ERR	MMPD_AUDIO_StartMIDINotePlay(void);
extern MMP_ERR	MMPD_AUDIO_SetEQType(MMP_USHORT usType, void* argHandler);
extern MMP_ERR	MMPD_AUDIO_SetPlayBuf(MMP_ULONG ulAddr,MMP_ULONG ulSize);
extern MMP_ERR  MMPD_AUDIO_SetPlayFileName(MMP_BYTE *bFileName, MMP_ULONG ulNameLength);
extern MMP_ERR  MMPD_AUDIO_SetPlayPath(MMPD_AUDIO_PLAY_DATAPATH ubPath);
extern MMP_ERR  MMPD_AUDIO_SetEncodePath(MMPD_AUDIO_RECORD_DATAPATH ubPath);
extern MMP_ERR  MMPD_AUDIO_SetEncodeFileName(MMP_BYTE *bFileName, MMP_ULONG ulNameLength);
extern MMP_ERR	MMPD_AUDIO_SetEncodeBuf(MMP_ULONG ulAddr,MMP_ULONG ulSize);
extern MMP_ERR	MMPD_AUDIO_SetLiveEncodeMode(MMP_USHORT usMode);
extern MMP_ERR	MMPD_AUDIO_GetLiveEncodeMode(MMP_USHORT *usMode);
extern MMP_ERR	MMPD_AUDIO_SetEncodeMode(MMP_USHORT usMode);
extern MMP_ERR	MMPD_AUDIO_GetEncodeMode(MMP_USHORT *usMode);
extern MMP_ERR  MMPD_AUDIO_GetRecordSize(MMP_ULONG *ulSize);
extern MMP_ERR  MMPD_AUDIO_GetRecordStatus(MMP_USHORT *usStatus);
extern MMP_ERR  MMPD_AUDIO_GetLiveRecordStatus(MMP_USHORT *usStatus);
extern MMP_ERR  MMPD_AUDIO_GetRecordFrameCntInSeconds(MMP_ULONG ulSec, MMP_ULONG *ulCnt);

extern MMP_ERR  MMPD_AUDIO_GetWriteCardStatus(MMP_USHORT *usStatus);
extern MMP_ERR	MMPD_AUDIO_GetPlayFileSize(MMP_ULONG *ulFileSize);
extern MMP_ERR	MMPD_AUDIO_UpdatePlayReadPtr(void);
extern MMP_ERR	MMPD_AUDIO_UpdatePlayWritePtr(void);
extern MMP_ERR	MMPD_AUDIO_SetPlayFileSize(MMP_ULONG ulFileSize);
extern MMP_ERR  MMPD_AUDIO_TransferDataToAIT(MMP_UBYTE *ubAudioPtr, MMP_ULONG ulMaxSize, 
				MMP_ULONG *ulReturnSize);
extern MMP_ERR	MMPD_AUDIO_InitPlayRWPtr(void);
extern MMP_ERR	MMPD_AUDIO_SetPlayHandshakeBuf(MMP_ULONG addr,MMP_ULONG ulSize);
extern MMP_ERR	MMPD_AUDIO_SetRecHandshakeBuf(MMP_ULONG ulAddr,MMP_ULONG ulSize);
extern MMP_ERR	MMPD_AUDIO_UpdateRecWritePtr(void);
extern MMP_ERR  MMPD_AUDIO_TransferDataToHost(MMP_UBYTE *ubAudioPtr, MMP_ULONG ulMaxSize, 
				MMP_ULONG *ulReturnSize);
extern MMP_ERR	MMPD_AUDIO_InitRecRWPtr(void);
extern MMP_ERR	MMPD_AUDIO_SetSBCEncodeEnable(MMP_UBYTE ubEnable,MMP_ULONG ulIntPeriod);
extern MMP_ERR	MMPD_AUDIO_GetSBCEncodeEnable(MMP_BOOL *bEnable);
extern MMP_ERR	MMPD_AUDIO_SetSBCChannelMode(MMP_USHORT usMode);
extern MMP_ERR	MMPD_AUDIO_SetSBCAllocMethod(MMP_USHORT usMethod);
extern MMP_ERR	MMPD_AUDIO_SetSBCNrofBlocks(MMP_USHORT usBlocks);
extern MMP_ERR	MMPD_AUDIO_SetSBCNrofSubbands(MMP_USHORT usSubBands);
extern MMP_ERR	MMPD_AUDIO_SetSBCBitrate(MMP_USHORT usBitRate);
extern MMP_ERR	MMPD_AUDIO_SetSBCBitPool(MMP_USHORT usBitPool);
extern MMP_ERR	MMPD_AUDIO_SetSBCEncBuf(MMP_ULONG ulBufAddr, MMP_ULONG ulBufSize);
extern MMP_ERR  MMPD_AUDIO_GetSBCInfo(  MMP_ULONG *ulOutSampleRate, 
                                        MMP_ULONG *ulOutBitRate,
                                        MMP_ULONG *ulOutFrameSize,
                                        MMP_ULONG *ulSBCFrameCount,
                                        MMP_ULONG *ulSBCSamplesPerFrame,
                                        MMP_ULONG *ulSBCChannelNum,
                                        MMP_ULONG *ulSBCBitpool);

extern MMP_ERR MMPD_AUDIO_SetAC3VHPar(MMP_LONG lm3deff, MMP_LONG ldb, MMP_BOOL bSet_m3deff, MMP_BOOL bSet_db);

extern MMP_ERR	MMPD_AUDIO_SetWAVEncodeEnable(MMP_UBYTE ubEnable);
extern MMP_ERR  MMPD_AUDIO_GetWAVInfo(MMP_ULONG *ulOutSampleCount);

extern MMP_ERR  MMPD_AUDIO_TransferMIDIDataToAIT(MMP_UBYTE *audio_ptr, MMP_ULONG ulFileSize);
extern MMP_ERR	MMPD_AUDIO_SetABRepeatMode(MMP_USHORT point , MMP_ULONG pos);
extern MMP_ERR	MMPD_AUDIO_GetPlayPosition(MMP_ULONG *pos);
extern MMP_ERR	MMPD_AUDIO_SetPlaySpectrumBuf(MMP_ULONG ulAddr,MMP_ULONG ulSize);
extern MMP_ERR	MMPD_AUDIO_GetPlaySpectrum(MMP_USHORT *usSpectrum);
extern MMP_ERR	MMPD_AUDIO_AACPlusPlayTime2FileOffset(MMP_ULONG ulMilliSec,MMP_ULONG *ulFileOffset);
extern MMP_ERR	MMPD_AUDIO_MP3PlayTime2FileOffset(MMP_ULONG ulMilliSec,MMP_ULONG *ulFileOffset);
extern MMP_ERR  MMPD_AUDIO_WAVPlayTime2FileOffset(MMP_ULONG ulMilliSec, MMP_ULONG *ulFileOffset);
extern MMP_ERR	MMPD_AUDIO_AMRPlayTime2FileOffset(MMP_ULONG ulMilliSec,MMP_ULONG *ulFileOffset);
extern MMP_ERR	MMPD_AUDIO_SaveAACRecParameter(MMP_ULONG ulMode);
extern MMP_ERR	MMPD_AUDIO_ReadAACRecParameter(MMP_ULONG *ulBitRate, MMP_USHORT *usSampleRate);
extern MMP_ERR	MMPD_AUDIO_SaveMP3RecParameter(MMP_ULONG ulMode);
extern MMP_ERR	MMPD_AUDIO_ReadMP3RecParameter(MMP_ULONG *ulBitRate, MMP_USHORT *usSampleRate);
extern MMP_ERR	MMPD_AUDIO_SaveAMRRecParameter(MMP_ULONG ulMode);
extern MMP_ERR	MMPD_AUDIO_ReadAMRRecParameter(MMP_ULONG *ulBitRate);
extern MMP_ERR  MMPD_AUDIO_SaveWAVRecParameter(MMP_ULONG ulMode);
extern MMP_ERR  MMPD_AUDIO_ReadWAVRecParameter(MMP_USHORT *usSampleRate);
extern MMP_ERR	MMPD_AUDIO_GetPlayStartSeekFlag(MMP_USHORT *usFlag);
extern MMP_ERR	MMPD_AUDIO_GetPlayFinishSeekFlag(MMP_USHORT *usFlag);
extern MMP_ERR	MMPD_AUDIO_SetPlayStartSeekFlag(MMP_USHORT usFlag);
extern MMP_ERR	MMPD_AUDIO_GetPlayFileSeekOffset(MMP_ULONG *ulOffset);
extern MMP_ERR	MMPD_AUDIO_UpdatePlayStreamSize(MMP_ULONG ulFileSize);
extern MMP_ERR	MMPD_AUDIO_UpdatePlayFileRemainSize(MMP_ULONG ulFileSize);
extern MMP_ERR	MMPD_AUDIO_InitPlaySeekFlag(void);
extern MMP_ERR  MMPD_AUDIO_SetAudioTagBuf(MMP_ULONG ulTagAddr,MMP_ULONG ulTagSize);
extern MMP_ERR  MMPD_AUDIO_GetWMATagLen(MMP_ULONG *Len, MMPD_AUDIO_WMA_INFO *wmaproinfo);
extern MMP_ERR  MMPD_AUDIO_GetOGGFileInfo(MMPD_AUDIO_OGG_INFO *ogginfo);
extern MMP_ERR  MMPD_AUDIO_GetOGGTagInfo(MMPD_AUDIO_OGG_TAG_INFO *TagInfo);

extern MMP_ERR  MMPD_AUDIO_SetPlayFileNameBuf(MMP_ULONG ulBufAddr);
extern MMP_ERR  MMPD_AUDIO_SetEncodeFileNameBuf(MMP_ULONG ulBufAddr);
extern MMP_ERR	MMPD_AUDIO_GetAudioPlayBufSize(MMP_ULONG *ulSize);
extern MMP_ERR	MMPD_AUDIO_SetRecReadPtr(void);
extern MMP_ERR	MMPD_AUDIO_SetPlayIntThreshold(MMP_USHORT usThreshold);
extern MMP_ERR	MMPD_AUDIO_SetRecordIntThreshold(MMP_USHORT usThreshold);
extern MMP_ERR  MMPD_AUDIO_SetGraphicEQEnable(MMP_BOOL ubEnable);
extern MMP_ERR	MMPD_AUDIO_SetGraphicEQBand(MMP_USHORT usFreq, MMP_USHORT usQrange, MMP_USHORT usGain);
extern MMP_ERR  MMPD_AUDIO_SetForeignEffectEnable(MMP_BOOL ubEnable);
extern MMP_ERR  MMPD_AUDIO_SetForeignEffectType(MMP_ULONG ulParam1, MMP_ULONG ulParam2, MMP_ULONG ulParam3);
extern MMP_ERR  MMPD_AUDIO_StartPreview(MMP_ULONG ulStartTime, MMP_ULONG ulDuration);
extern MMP_ERR  MMPD_AUDIO_GetPreviewStatus(MMP_BOOL *bDataReady, MMP_SHORT **usDataBuf, MMP_ULONG *ulSampleNum);
extern MMP_ERR	MMPD_AUDIO_SetDuplexPath(MMP_USHORT usPath);
extern MMP_ERR	MMPD_AUDIO_SetPlayFileSize(MMP_ULONG ulFileSize);
extern MMP_ERR	MMPD_AUDIO_SetEncodeFileSize(MMP_ULONG ulSize);
extern MMP_ERR  MMPD_AUDIO_DACChargeSpeed(MMP_BOOL bFastCharge);
extern MMP_ERR  MMPD_AUDIO_PreDACPowerOn(MMP_ULONG ulSampleRate);
extern MMP_BOOL MMPD_Audio_GetDACPowerOnStatus(void);
extern MMP_BOOL MMPD_Audio_GetADCPowerOnStatus(void);

MMP_ERR	MMPD_AUDIO_SetAudioOutI2SFormat(MMP_UBYTE ch, MMPD_AUDIO_I2S_FORMAT i2sformat);
MMP_ERR	MMPD_AUDIO_SetAudioInI2SFormat(MMP_UBYTE ch, MMPD_AUDIO_I2S_FORMAT i2sformat);
MMP_ERR	MMPD_AUDIO_SetI2SWorkMode(MMP_UBYTE ch, MMPD_AUDIO_I2S_WORK_MODE workingMode);
MMP_ERR	MMPD_AUDIO_SetI2SMclkMode(MMP_UBYTE ch, MMPD_AUDIO_I2S_MCLK_MODE mclkMode);
MMP_ERR	MMPD_AUDIO_SetI2SOutBitWidth(MMP_UBYTE ch, MMPD_AUDIO_I2S_OUT_BITS outputBitSize);
MMP_ERR	MMPD_AUDIO_SetI2SInBitWidth(MMP_UBYTE ch, MMPD_AUDIO_I2S_IN_BITS inputBitSize);
MMP_ERR	MMPD_AUDIO_SetI2SLRckPolarity(MMP_UBYTE ch, MMPD_AUDIO_I2S_WS_POLARITY lrckPolarity);
MMP_ERR	MMPD_AUDIO_SetI2SOutDataDelayMode(MMP_UBYTE ch, MMPD_AUDIO_I2S_DELAY_MODE outDataDelayMode);
MMP_ERR	MMPD_AUDIO_SetI2SOutBySDI(MMP_UBYTE ch, MMP_BOOL bOutputBySDI);
MMP_ERR	MMPD_AUDIO_SetI2SInFromSDO(MMP_UBYTE ch, MMP_BOOL bInputFromSDO);
MMP_ERR	MMPD_AUDIO_SetI2SInBitAlign(MMP_UBYTE ch, MMP_UBYTE ubInBitAlign);
MMP_ERR	MMPD_AUDIO_SetI2SOutBitAlign(MMP_UBYTE ch, MMP_UBYTE ubOutBitAlign);
MMP_ERR	MMPD_AUDIO_SetI2SMclkFreq(MMP_UBYTE ch, MMPD_AUDIO_I2S_MCLK_MODE mclkMode, MMP_ULONG ulSamplerate, MMP_ULONG ulFixedMclkFreq);
MMP_ERR	MMPD_AUDIO_EnableI2SMclk(MMP_UBYTE ch, MMPD_AUDIO_I2S_WORK_MODE workMode, MMPD_AUDIO_I2S_MCLK_MODE mclkMode, MMP_BOOL bEnable);

MMP_ERR	MMPD_AUDIO_SetEncodeFormat(MMPD_AUDIO_ENCODE_FMT encodefmt);
MMP_ERR	MMPD_AUDIO_SetLiveEncodeFormat( MMPD_AUDIO_ENCODE_FMT   encodefmt,
                                        MMP_ULONG               inSamplerate,
                                        MMP_ULONG               outSamplerate,
                                        MMP_UBYTE               inCh,
                                        MMP_UBYTE               outCh);
MMP_ERR	MMPD_AUDIO_SetPlayFormat(MMPD_AUDIO_DECODE_FMT decodefmt);
MMP_ERR	MMPD_AUDIO_SetInPath(MMP_ULONG audiopath);
MMP_ERR	MMPD_AUDIO_SetOutPath(MMP_ULONG audiopath);
MMP_ERR	MMPD_AUDIO_SetLineInChannel(MMP_AUD_LINEIN_CH lineinchannel);
MMP_ERR MMPD_AUDIO_SetBypassPath(MMP_UBYTE bypasspath);

MMP_ERR	MMPD_AUDIO_SetPlayReadPtr(MMP_ULONG usAudioPlayReadPtr, MMP_ULONG usAudioPlayReadPtrHigh);
MMP_ERR	MMPD_AUDIO_SetPlayWritePtr(MMP_ULONG usAudioPlayWritePtr, MMP_ULONG usAudioPlayWritePtrHigh);
MMP_ERR MMPD_AUDIO_TransferDataHostToDev(void);
MMP_ERR MMPD_AUDIO_InitialMemModePointer(MMP_UBYTE *ubBufAddr, MMP_ULONG ulBufSize, MMP_ULONG ulReadSize, MMP_ULONG ulBufThreshold);
MMP_ERR MMPD_AUDIO_SetMemModeReadPointer(MMP_ULONG ulMemReadPtr, MMP_ULONG ulMemReadPtrHigh);
MMP_ERR MMPD_AUDIO_TransferDataCardToHost(void* FileHandle);
MMP_ERR MMPD_AUDIO_SetPLL(MMP_USHORT usSampleRate);
MMP_ERR MMPD_AUDIO_SetGaplessEnable(MMP_BOOL bEnable, void *nextFileCB, MMP_ULONG param1);
MMP_ERR MMPD_AUDIO_GetGaplessAllowedOp(MMP_USHORT *op);

MMP_ERR MMPD_AUDIO_FlushMp3Cache(void);
MMP_ERR MMPD_AUDIO_InitializeMp3ZIRegion(void);
MMP_ERR MMPD_AUDIO_FlushAudioCache(MMP_ULONG ulRegion);
MMP_ERR MMPD_AUDIO_InitializeAudioZIRegion(MMP_ULONG ulRegion);
MMP_ERR MMPD_AUDIO_TransferAudioDataToHost(MMP_UBYTE *ubAudioPtr, MMP_ULONG ulMaxSize, MMP_ULONG ulFrameSize,
                MMP_ULONG *ulReturnSize);
MMP_ERR	MMPD_AUDIO_SetMemModeFileHostInfo(void* FileHandle, MMP_ULONG ulAudioPlayFileSize);

MMP_ERR	MMPD_AUDIO_SetPlayDigitalGain(MMP_UBYTE ubDGain);
MMP_ERR	MMPD_AUDIO_SetPlayAnalogGain(MMP_UBYTE ubAGain);
MMP_ERR	MMPD_AUDIO_SetRecordDigitalGain(MMP_UBYTE ubDGain);
MMP_ERR MMPD_AUDIO_EnableDummyRecord(MMP_BOOL bEnable);
MMP_ERR MMPD_AUDIO_SetRecordSilence(MMP_BOOL bEnable);
MMP_ERR	MMPD_AUDIO_SetRecordAnalogGain(MMP_UBYTE ubAGain, MMP_BOOL bboostup);
MMP_ERR MMPD_AUDIO_GetPlayFormat(MMPD_AUDIO_DECODE_FMT *format);
MMP_ERR MMPD_AUDIO_GetEncodeFormat(MMPD_AUDIO_ENCODE_FMT *fmt);
MMP_ERR MMPD_AUDIO_GetLiveEncodeFormat(MMPD_AUDIO_ENCODE_FMT *fmt);

MMP_ERR	MMPD_AUDIO_SetParseFormat(MMPD_AUDIO_DECODE_FMT fmt);
MMP_ERR MMPD_AUDIO_SetParseFileName(MMP_BYTE *bFileName, MMP_ULONG ulNameLength);
MMP_ERR MMPD_AUDIO_SetParseFileNameBuf(MMP_ULONG ulBufAddr);
MMP_ERR MMPD_AUDIO_SetParseBuf(MMP_ULONG ulAddr, MMP_ULONG ulSize);
MMP_ERR MMPD_AUDIO_SetParsePath(MMPD_AUDIO_PLAY_DATAPATH ubPath);
MMP_ERR MMPD_AUDIO_PresetPlayFileNameBuf(MMP_ULONG ulBufAddr);

MMP_ERR MMPD_AUDIO_SetLiveEncodeCB(MMP_LivePCMCB  LivePCMCB);
extern MMP_ERR	MMPD_AUDIO_StartAudioRecordEx(  MMP_BOOL                bPreEncode,
                                                MMPD_AUDIO_ENCODE_FMT   encodefmt,
                                                MMP_ULONG               samplerate);
extern MMP_ERR  MMPD_AUDIO_DoLiveEncodeOp(MMP_USHORT op);
extern MMP_ERR  MMPD_AUDIO_GetEncodeBuf(MMP_ULONG* ulAddr, MMP_ULONG* ulSize);

extern MMP_ERR  MMPD_AUDIO_ConfigWNR(MMPD_AUDIO_WNR_CFG *cfg);
extern MMP_ERR  MMPD_AUDIO_SetWNREnable(MMP_BOOL enable);

extern MMP_ERR  MMPD_AUDIO_SetAGCEnable(MMP_BOOL enable);

/// @}
#endif // _MMPD_AUDIO_H_
/// @end_ait_only

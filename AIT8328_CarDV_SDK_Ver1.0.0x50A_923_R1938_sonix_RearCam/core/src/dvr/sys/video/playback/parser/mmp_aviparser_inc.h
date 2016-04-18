/**
 @file mmp_aviparser_inc.h
 @brief A common 3GP demuxer
 
 A common 3GP demuxer file which is both used in firmware and the host side code.
 
 This is a very special .h file. It's an inline c file. We usually use .inc file extention
 but it would need .mcp file to add one more file.
 Because this is a common file used in firmware and the host side, we don't know
 which include file it need, such as mmp_lib.h and includes_fw.h. So I make it as a header file
 form.
 
 @author Truman Yang
 @version 1.0 Original Version
*/

#ifndef _MMP_AVIPARSER_INC_H_
#define _MMP_AVIPARSER_INC_H_

#if !defined(BUILD_FW)||(VIDEO_P_EN)
#ifdef BUILD_FW
#include "mmp_aviparser.h"
#if (VWAV_P_EN)
#include "mmpf_wavdecapi.h"
#include "adpcmdec_api.h"
#endif
#if (VWMA_P_EN)
#include "mmpf_wmaprodecapi.h"
#endif
#if (AIT_DIVXDRM_SUPPORT == 1)
#include "mmpf_drm_api.h"
#endif

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

// AVI Header flag
#define AVIF_HASINDEX               0x00000010        // Index at end of file?
#define AVIF_MUSTUSEINDEX           0x00000020
#define AVIF_ISINTERLEAVED          0x00000100
#define AVIF_TRUSTCKTYPE            0x00000800        // Use CKType to find key frames?
#define AVIF_WASCAPTUREFILE         0x00010000
#define AVIF_COPYRIGHTED            0x00020000

#define AVIIF_LIST                  0x00000001
#define AVIIF_KEYFRAME              0x00000010
#define AVIIF_NOTIME                0x00000100

#define AVI_SAMPLESIZEMASK          0x7fffffff
#define AVI_UNDEFINESAMPLENUM       0x7fffffff
#define AVI_ISKEYFRAME              0x80000000

#define MMP_TAG_WMA1                0x160
#define MMP_TAG_WMA2                0x161
#define MMP_TAG_WMA3                0x162

#define SPEED_UP_AVI_OPEN           (1)
#define SPEED_UP_AVI_SEEK           (1)
// Jump super index table to speed up seek, it maybe cause avi seek cannot find key frame
#define SPEED_UP_AVI_SPIDX_SEEK     (1)
// For some broken avi file, we need use index table to find correct frame position
#define KEEP_AVI_INDEX_POSTION      (1)
#define SUPPORT_NO_IFLAG_SEEK       (1)

#define AVI_INDEX_BUFFER_SIZE       (256*1024)
#define AVI_MIN_INDEX_DURATION      (500) // 500 ms
#define AVI_MAX_SEARCH_SAMPLE_COUNT (500)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_AVIPSR_CACHE_INDEX {
    INIT_CACHE      = 0,
    AVI_VIDEO_CACHE,
    AVI_AUDIO_CACHE,
    AVI_VIDEO_INDEX_CACHE,  // video index table cache index 
    AVI_AUDIO_INDEX_CACHE,  // audio index table cache index 
    AVI_VIDEO_SPIDX_CACHE,  // video super index table cache index
    AVI_AUDIO_SPIDX_CACHE   // audio super index table cache index
} MMPF_AVIPSR_CACHE_INDEX;

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/** @brief FourCC type in AVI
*/

static const char avi_headers[][8] = {
    { 'R', 'I', 'F', 'F',    'A', 'V', 'I', ' ' },
    { 'R', 'I', 'F', 'F',    'A', 'V', 'I', 'X' },
    { 'R', 'I', 'F', 'F',    'A', 'V', 'I', 0x19},
    { 'O', 'N', '2', ' ',    'O', 'N', '2', 'f' },
    { 'R', 'I', 'F', 'F',    'A', 'M', 'V', ' ' },
    { 0 }
};

static const AVCodecTag codec_bmp_tags[] = {
    { MMP_VID_VIDEO_MJPG,   MKTAG('M', 'J', 'P', 'G') },
    { MMP_VID_VIDEO_H264,   MKTAG('H', '2', '6', '4') },
    { MMP_VID_VIDEO_H264,   MKTAG('h', '2', '6', '4') },
    { MMP_VID_VIDEO_H264,   MKTAG('X', '2', '6', '4') },
    { MMP_VID_VIDEO_H264,   MKTAG('x', '2', '6', '4') },
    { MMP_VID_VIDEO_H264,   MKTAG('a', 'v', 'c', '1') },
    { MMP_VID_VIDEO_H264,   MKTAG('V', 'S', 'S', 'H') },
    { MMP_VID_VIDEO_H263,   MKTAG('H', '2', '6', '3') },
    { MMP_VID_VIDEO_H263,   MKTAG('X', '2', '6', '3') },
    { MMP_VID_VIDEO_H263,   MKTAG('T', '2', '6', '3') },
    { MMP_VID_VIDEO_H263,   MKTAG('L', '2', '6', '3') },
    { MMP_VID_VIDEO_H263,   MKTAG('V', 'X', '1', 'K') },
    { MMP_VID_VIDEO_H263,   MKTAG('Z', 'y', 'G', 'o') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('F', 'M', 'P', '4') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('D', 'I', 'V', 'X') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('D', 'X', '5', '0') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('X', 'V', 'I', 'D') },
    { MMP_VID_VIDEO_MP4V,   MKTAG( 4 ,  0 ,  0 ,  0 ) }, /* some broken avi use this */
    { MMP_VID_VIDEO_MP4V,   MKTAG('m', 'p', '4', 'v') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('U', 'M', 'P', '4') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('P', 'M', '4', 'V') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('S', 'M', 'P', '4') },
    { MMP_VID_VIDEO_DIVX,   MKTAG('D', 'I', 'V', '3') },
    { MMP_VID_VIDEO_DIVX,   MKTAG('D', 'I', 'V', '5') },
    { MMP_VID_VIDEO_DIVX,   MKTAG('D', 'I', 'V', '6') },
    { MMP_VID_VIDEO_DIVX,   MKTAG('D', 'I', 'V', '4') },
    { MMP_VID_VIDEO_DIVX,   MKTAG('D', 'V', 'X', '3') }, 
    { MMP_VID_VIDEO_WMV,    MKTAG('W', 'M', 'V', '3') },
    { MMP_VID_VIDEO_MP4V,   MKTAG('M', 'P', '4', 'V') },
    { MMP_VID_VIDEO_UNSUPPORTED,         0 }
};

static const AVCodecTag codec_wav_tags[] = {
    { MMP_VID_AUDIO_WAV,    0x0001 },
    { MMP_VID_AUDIO_MP3,    0x0055 }, // assume it is AAC now, actually, it is MP3
    { MMP_VID_AUDIO_MP3,    0x0050 },
    { MMP_VID_AUDIO_AMR_NB, 0x0057 },
    { MMP_VID_AUDIO_AAC,    0x00ff },
    { MMP_VID_AUDIO_WMA,    0x0160 },
    { MMP_VID_AUDIO_WMA,    0x0161 },
    { MMP_VID_AUDIO_WMA,    0x0162 },
    { MMP_VID_AUDIO_WAV,    0x0002 },
    { MMP_VID_AUDIO_WAV,    0x0006 },
    { MMP_VID_AUDIO_WAV,    0x0007 },
    { MMP_VID_AUDIO_WAV,    0x0011 },
    { MMP_VID_AUDIO_WAV,    0x0045 },
    { MMP_VID_AUDIO_AAC,    0x706d },
    { MMP_VID_AUDIO_AAC,    0x4143 },
    { MMP_VID_AUDIO_AC3,    0x2000 },
    { 0, 0 },
};

//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

/** @name AVI Parser Variables
@{
*/

AVIHeader           avih;

MMP_AVI_MEDIA_INFO  gAVIVideoMedia[2];
MMP_AVI_MEDIA_INFO  gAVIAudioMedia;
MMP_AVI_MEDIA_INFO  gAVIDataMedia;
MMP_AVI_MEDIA_INFO  gAVITextMedia;
MMP_AVI_SAMPLE_INFO gAVIVideoSample[2];
MMP_AVI_SAMPLE_INFO gAVIAudioSample;

MMP_LONG            lAVIIndexEnd;
MMP_ULONG           ulAudioBitRate;

MMP_ULONG           m_ulAVIVideoIdxPos[2];
MMP_ULONG           m_ulAVIAudioIdxPos;

#if (SPEED_UP_AVI_SEEK)
MMP_BOOL            m_bCountPreAudio;
MMP_BOOL            m_bCountPreVideo;
MMP_ULONG           m_ulPreAudioSize;
MMP_ULONG           m_ulPreVideoChunk;
MMP_BOOL            m_bCountAudioChunk;
MMP_ULONG           m_ulAudioChunkNum;
#endif

MMP_BOOL            m_bCheckUnknownChunk = MMP_FALSE;
MMP_ULONG           m_ulAudioJumpTime = 0;

#if (AIT_DIVXDRM_SUPPORT == 1)
#define AUDIO_TEMP_BS_SIZE (96 * 1024)
MMP_ULONG           m_ulAudioTempBuf;
MMP_ULONG           m_ulPreAudioChunkSize = 0;
#endif

MMP_BOOL            m_bBuildTableDone;
MMP_BOOL            m_bUseAudioSuperIdx;
MMP_ULONG           m_ulIDXPSRCacheAddr;
MMP_ULONG           m_ulIDXPSRCacheSize;
MMP_ULONG           m_ulIDXPSRPreSeekPos;
MMP_ULONG           m_ulIDXPSRfileID;
MMP_ULONG           m_ulIDXPSRfileSize;
MMP_ULONG           m_ulIDXTableEnd;
MMP_AVI_MEDIA_INFO  gIDXVideoMedia;
MMP_AVI_MEDIA_INFO  gIDXAudioMedia;
MMP_IDX_AVIMEDIA_INFO m_IndexContext;
#if (SUPPORT_NO_IFLAG_SEEK == 1)
MMP_ULONG           gulIframeSize[2] = {0};
MMP_BOOL            gubIdx1HasIflag[2] = {0};
#endif

#if (DEBUG_VIDEO & DUMP_BS_BUFDATA)
MMP_BOOL            m_bOpenFileOnetime = MMP_FALSE;
MMP_ULONG   		gulVidFileId;
#endif
MMP_BOOL            bIsUnknownAviData = MMP_FALSE;
//==============================================================================
//
//                              EXTERN VARIABLES
//
//==============================================================================

extern MMP_VID_VIDEOTYPE    m_MFDFormat;
extern MMP_ULONG            m_ulAudSampleRate;
extern MMP_BOOL             gbAllowListAtomEn;
extern MMP_ULONG            glAllowListAtom;
//==============================================================================
//
//                              FUNCTION PROTOTYPE
//
//==============================================================================

static  MMP_ERR     MMPF_VIDPSR_ReadFile(MMP_UBYTE *ptr, const MMP_ULONG SeekPos, const MMP_ULONG size);
MMP_ERR             MMPF_Player_GetSupportedFormat(MMP_VID_AUDIOTYPE *supported_audio_format,
                                                   MMP_VID_VIDEOTYPE *supported_video_format);
MMP_ERR             MMPF_AVIPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame);
MMP_ULONG           MMPF_AVIPSR_GetTime(MMP_AVI_MEDIA_INFO *pMedia);
#if (SUPPORT_NO_IFLAG_SEEK == 1)
MMP_ERR             MMPF_AVIPSR_SupportNoIfraflagSeek(MMP_ULONG ulIdx, MMP_ULONG ulTrack, MMP_ULONG ulLen, MMP_ULONG *ulFlags);
MMP_ERR             MMPF_AVIPSR_ChkFrameIfHasIflag(MMP_AVI_SAMPLE_INFO *pSample, MMP_ULONG *ulFlags);
#endif

#if (SPEED_UP_AVI_SEEK)
MMP_ERR             MMPF_IDXPSR_SeekKeySample(MMP_SHORT sAdvance, MMP_BOOL *bNoIndex, MMP_ULONG *ulIdxOffset, MMP_ULONG *ulSampleNum);
#endif

#if (DEBUG_VIDEO & DUMP_BS_BUFDATA)
MMP_ERR             MMPF_AVIPSR_DumpCompBuf(MMP_ULONG ulAddr,MMP_ULONG ulSize);
#endif
//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void _____AVIReadHeaderAPI_____(){}
#endif

static void MMPF_AVIPSR_SkipFile(MMP_ULONG offset)
{
	m_ulCurFileOffset += offset;
}

static MMP_ULONG MMPF_AVIPSR_GetLittleLong(const MMP_ULONG ulcurSeek, MMP_SHORT advance, MMP_ULONG *value)
{
	MMP_UBYTE buf[4];

	MMPF_VIDPSR_ReadFile(buf, ulcurSeek, 4);
	
	m_ulCurFileOffset += advance;
	
	*value = (((unsigned int) buf[3]) << 24) + 
	         (((unsigned int) buf[2]) << 16) + 
	         (((unsigned int) buf[1]) << 8) + 
             (unsigned int) buf[0];
	
	return *value;
}

static MMP_ULONG MMPF_AVIPSR_GetLittleShort(const MMP_ULONG ulcurSeek, MMP_SHORT advance, MMP_ULONG *value)
{
	MMP_UBYTE buf[2];

	MMPF_VIDPSR_ReadFile(buf, ulcurSeek, 2);
	
	m_ulCurFileOffset += advance;
	*value = 0;
	*value = (((MMP_ULONG) buf[1]) << 8) + (MMP_ULONG) buf[0];

	return *value;
}

static MMP_LONG MMPF_AVIPSR_GetRIFFHeader(AVIContext *AviCtxt)
{
    MMP_BYTE header[9];
    MMP_LONG i;

    /* Check RIFF header */
	header[8] = '\0';

    MMPF_VIDPSR_ReadFile((MMP_UBYTE *)header, m_ulCurFileOffset, 4);
    m_ulCurFileOffset += 4;
	MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4,&(AviCtxt->ulRiffEnd));   /* RIFF chunk size */

    AviCtxt->ulRiffEnd += m_ulCurFileOffset; /* RIFF chunk end */
    
	MMPF_VIDPSR_ReadFile((MMP_UBYTE *)(header+4), m_ulCurFileOffset, 4);
	m_ulCurFileOffset += 4;

    for (i = 0; avi_headers[i][0]; i++) {
        if (!memcmp(header, avi_headers[i], 8))
            break;
    }
            
    if (!avi_headers[i][0]) {
        return -1;
    }
    return 0;
}

static void MMPF_AVIPSR_ToUpper(MMP_UBYTE *tags, MMP_ULONG size)
{
    MMP_ULONG i;
    
    for (i = 0; i < size; i++) {
        if (tags[i] >= 0x61)
            tags[i] -= 0x20;
    }
}

static MMP_ULONG MMPF_AVIPSR_GetCodecType(const AVCodecTag *Tag_Table, MMP_ULONG Name)
{
	MMP_LONG i;
	
	for(i = 0; Tag_Table[i].tag != 0; i++) {
        if (Name == Tag_Table[i].tag)
            return Tag_Table[i].id;
    }
    return 0x100; //unsupported
}

static void MMPF_AVIPSR_GetAudioHeader(MMP_ULONG size)
{
	MMP_ULONG id;
	MMP_ULONG tmp;
	MMP_ULONG ulOpt1 = 0, ulOpt2 = 0, ulOpt3;
	MMP_ULONG ulExtraData = 0;

	MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);

    #if (VWAV_P_EN)
	MMPF_SetWAVFormat(MMP_FALSE);
	#endif

	id = tmp & 0xffff;
	
	gVidFileInfo.audio_info.channel_nums = tmp >> 16;
	MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &gVidFileInfo.audio_info.sampling_rate);
	MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);
	gVidFileInfo.audio_info.bitrate = tmp * 8;
	ulAudioBitRate = gVidFileInfo.audio_info.bitrate;
	MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset, 2, &tmp);
	gVidFileInfo.audio_info.block_align = tmp;
	
	if (size == 14) {
		gVidFileInfo.audio_info.bits_per_coded_sample = 8;
	} 
	else {
		MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset, 2, &gVidFileInfo.audio_info.bits_per_coded_sample);
	}

	if (size >= 18)
	{
		MMP_ULONG cbsize;
		MMP_ULONG ulExtraSize = 0;

		MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset, 2, &cbsize);
		
		size -= 18;
		cbsize = FFMIN(size, cbsize);
        
        ulExtraSize = cbsize;
        
		if (cbsize >= 22 && id == 0xfffe) {
			MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset, 2, &gVidFileInfo.audio_info.bits_per_coded_sample);
			MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);   // channel mask
			MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &id);
			MMPF_AVIPSR_SkipFile(12);
			cbsize -= 22;
			size -= 22;
		}
		
		if (id == MMP_TAG_WMA1) {
		    MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset+2, 0, &ulOpt1);
		} 
		else if (id == MMP_TAG_WMA2) {
		    MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset+4, 0, &ulOpt1);
		} 
		else if (id == MMP_TAG_WMA3) {
		    MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset+10, 0, &ulOpt3);
		    MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset+14, 0, &ulOpt1);
		    MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset+16, 0, &ulOpt2);
		} 
		else if (ulExtraSize >= 2 && ulExtraSize < 22) {
		    MMPF_AVIPSR_GetLittleShort(m_ulCurFileOffset, 0, &ulExtraData);
		}
		
		if (size > 0) {
			MMPF_AVIPSR_SkipFile(size);
	    }
	}
	
	gVidFileInfo.audio_info.format = MMPF_AVIPSR_GetCodecType(codec_wav_tags, id);
	
	if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AAC) {
	    mp4ASC.objectTypeIndex = ((ulExtraData & 0xF1) >> 3);
	}
	
	#if (VWMA_P_EN)
	if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_WMA) {
	    WMAFormat *pWMAFormat = NULL;
	    
	    MMPF_WMADEC_GetWMAFormat(&pWMAFormat);

	    pWMAFormat->wFormatTag      = id;
	    pWMAFormat->nChannels       = gVidFileInfo.audio_info.channel_nums;
	    pWMAFormat->nSamplesPerSec  = gVidFileInfo.audio_info.sampling_rate;
	    pWMAFormat->nAvgBytesPerSec = (gVidFileInfo.audio_info.bitrate / 8);
	    pWMAFormat->nBlockAlign     = gVidFileInfo.audio_info.block_align;
	    pWMAFormat->nValidBitsPerSample =  gVidFileInfo.audio_info.bits_per_coded_sample;
	    
	    if (pWMAFormat->nValidBitsPerSample == 0) {
	        pWMAFormat->nValidBitsPerSample = 16;
	    }
	        
	    if (pWMAFormat->nChannels == 1) {
	        pWMAFormat->nChannelMask = 1;
	    } 
	    else if (pWMAFormat->nChannels == 2) {
	        pWMAFormat->nChannelMask = 3;
	    }

        pWMAFormat->wEncodeOpt = ulOpt1;
        pWMAFormat->wAdvancedEncodeOpt = ulOpt2;
        pWMAFormat->dwAdvancedEncodeOpt2 = ulOpt3;
	}
	#endif

    #if (VMP3_P_EN)
	if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_MP3) {
	    if (id == 0x0050)
	        MMPF_SetMP3Layer(0x02);
	    else 
	        MMPF_SetMP3Layer(0x03);
	}
	#endif

    #if (VWAV_P_EN)
	if ((gVidFileInfo.audio_info.format == MMP_VID_AUDIO_WAV) && (id != 0x0001)/*not pcm*/){
	    
	    MMP_ULONG ulBitPerSample = 0;
	    AIT_AdpcmDecCtx *tContext;
	    
	    if (id == 0x0045) { //G726
	        ulBitPerSample = (gVidFileInfo.audio_info.bitrate + gVidFileInfo.audio_info.sampling_rate/2)/gVidFileInfo.audio_info.sampling_rate;
	    } 
	    else {
	        ulBitPerSample = gVidFileInfo.audio_info.bits_per_coded_sample;
	    }
	    
	    if ((id == 0x0045 || id == 0x0011) && (ulBitPerSample != 4)) {
	        gVidFileInfo.audio_info.format = MMP_VID_AUDIO_UNSUPPORTED;
	        gVidFileInfo.is_audio_available = 0;
	        return;
	    }
	    
	    if (gVidFileInfo.audio_info.block_align == 0) {
	        gVidFileInfo.audio_info.format = MMP_VID_AUDIO_UNSUPPORTED;
	        gVidFileInfo.is_audio_available = 0;
	        return;
	    }

	    MMPF_SetWAVFormat(MMP_TRUE);
	    
	    MMPF_GetWAVContext(&tContext);
	    
	    tContext->AdpcmDCtxnCh              = gVidFileInfo.audio_info.channel_nums;
	    tContext->AdpcmDCtxBlkAlign         = gVidFileInfo.audio_info.block_align;
	    tContext->AdpcmDCtxCID              = id;
	    tContext->AdpcmDCtxBitPerEncSample  = gVidFileInfo.audio_info.bits_per_coded_sample;
	    tContext->AdpcmDCtxSampleRate       = gVidFileInfo.audio_info.sampling_rate;
	    tContext->AdpcmDCtxBitRate          = gVidFileInfo.audio_info.bitrate;
	}
	#endif
	
	if(gVidFileInfo.audio_info.format == MMP_VID_AUDIO_UNSUPPORTED) {
	    gVidFileInfo.is_audio_available = 0;
	}    
}

/** @} */ // end of AVI Parser API

#if 0
void _____AVIDemuxer_____(){}
#endif

MMP_ULONG MMPF_AVIPSR_SetIndex(MMP_ULONG ulData, MMP_ULONG ulIdx, MMP_ULONG ulFileSize, MMP_ULONG ulTargetIdx)
{
	MMP_ULONG   tag, size;
	MMP_ULONG   nb_entries, i;
	MMP_ULONG   tag1, flags, pos, len, index;
	MMP_LONG    index_pos;
	MMP_UBYTE   ubVidFinded = 0, ubAudFinded = 0;
#if (AIT_DIVXDRM_SUPPORT == 1)	
	MMP_ULONG   idxtablestart = 0;
#endif
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoDmxTrackNo];    
	m_ulCurFileOffset = ulIdx;
	
    while(m_ulCurFileOffset < ulFileSize)
	{
        //if (m_ulCurFileOffset >= ulFileSize) {
        //    goto the_end;
        //}
            
		MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tag);
		MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &size);
		
		if (tag == 0 && size == 0) {
		    return 0;
		}

		switch(tag){
		case MKTAG('i', 'd', 'x', '1'):
			nb_entries = size / 16;
			lAVIIndexEnd = m_ulCurFileOffset + size;

			if (pVidMedia->ulTotalFrame != AVI_UNDEFINESAMPLENUM && 
			    gAVIAudioMedia.ulTotalFrame == AVI_UNDEFINESAMPLENUM) {
			    gAVIAudioMedia.ulTotalFrame = nb_entries - pVidMedia->ulTotalFrame;
			}
            
            #if (AIT_DIVXDRM_SUPPORT == 1)			
			idxtablestart = m_ulCurFileOffset;
            #endif
            			
            pVidMedia->ulTotalEntry = nb_entries;
            gAVIAudioMedia.ulTotalEntry = nb_entries;
            pVidMedia->ulInitialIndex = m_ulCurFileOffset;
            gAVIAudioMedia.ulInitialIndex = m_ulCurFileOffset;
            
			for(i = 0; i < nb_entries; i++)
			{
				index_pos = m_ulCurFileOffset;
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tag1);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &flags);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pos);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &len);
				
				if (i == 0) {
				    if (pVidMedia->ulTrackID == ulTargetIdx) {
        				pVidMedia->ulIndexMoviOffset = pVidMedia->ulMoviOffset;
        				if (pos > ulData) {
        					pVidMedia->ulIndexMoviOffset = 0;
        				}
    				} 
    				else if (gAVIAudioMedia.ulTrackID == ulTargetIdx) {
        			    gAVIAudioMedia.ulIndexMoviOffset = gAVIAudioMedia.ulMoviOffset;
        				if (pos > ulData) {
        					gAVIAudioMedia.ulIndexMoviOffset = 0;
        				}
    				}
				}
				
				index = ((tag1 & 0xff) - '0') * 10;
				index += ((tag1 >> 8) & 0xff) - '0';
				
				if (index == pVidMedia->ulTrackID && index == ulTargetIdx && !ubVidFinded && (((tag1 >> 16) & 0xff) == 'd') && ((((tag1 >> 24) & 0xff) == 'c') || (((tag1 >> 24) & 0xff) == 'b'))){

                    #if (AIT_DIVXDRM_SUPPORT == 1)
					pVidMedia->ulIdxOffset = idxtablestart;
					m_ulAVIVideoIdxPos[m_ubVideoCurTrack] = idxtablestart;
                    #else
					pVidMedia->ulIdxOffset = index_pos;
					m_ulAVIVideoIdxPos[m_ubVideoCurTrack] = index_pos;
                    #endif	
					ubVidFinded = 1;
				}
				
				if (index == gAVIAudioMedia.ulTrackID && index == ulTargetIdx && !ubAudFinded && (((tag1 >> 16) & 0xff) == 'w') && (((tag1 >> 24) & 0xff) == 'b')){

                    #if (AIT_DIVXDRM_SUPPORT == 1)
					gAVIAudioMedia.ulIdxOffset = idxtablestart;
					m_ulAVIAudioIdxPos = idxtablestart;
                    #else
					gAVIAudioMedia.ulIdxOffset = index_pos;
					m_ulAVIAudioIdxPos = index_pos;
                    #endif	

					ubAudFinded = 1;
				}
				if(ubVidFinded || ubAudFinded) {
					goto the_end;
			    }
			}
			break;
		default:
			size += (size & 1);
			MMPF_AVIPSR_SkipFile(size);
			break;
		}
	}
the_end:
	return 0;
}

MMP_ERR MMPF_AVIPSR_GetNextSuperIdx(MMP_AVI_MEDIA_INFO *pMedia, MMP_SHORT nAdvance)
{
    MMP_ERR err = MMP_ERR_NONE;
    MMP_ULONG ulHighL, ulLowL;
    MMPF_AVIPSR_CACHE_INDEX CacheType;
    
    if ((pMedia->ulSpIdxCount >= pMedia->ulSpIdxNum) && (nAdvance > 0)) {
        return MMP_VIDPSR_ERR_EOS;
    }
    
    if ((pMedia->ulSpIdxCount <= 1) && (nAdvance < 0)){
        return MMP_VIDPSR_ERR_EOS;
    }
    
	if (pMedia->ulCodecType == CODEC_TYPE_VIDEO) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_SPIDX_CACHE);
	    CacheType = AVI_VIDEO_INDEX_CACHE;
	} 
	else if (pMedia->ulCodecType == CODEC_TYPE_AUDIO) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_AUDIO_SPIDX_CACHE);
	    CacheType = AVI_AUDIO_INDEX_CACHE;
	} 
	else {
	    return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
	}
	
    if (nAdvance < 0) {
        pMedia->ulSpIdxOffset -= 32;
    }
    
    if ((pMedia->ulSpIdxOffset + 16) > pMedia->ulFileSize) {
        return MMP_VIDPSR_ERR_EOS;
    }
        
	MMPF_AVIPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &ulLowL);
	pMedia->ulSpIdxOffset += 4;
	MMPF_AVIPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &ulHighL);
	pMedia->ulSpIdxOffset += 4;
	pMedia->SpIdxEntry.ullOffset = ((MMP_ULONG64)ulHighL << 32) | ulLowL;
	MMPF_AVIPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &pMedia->SpIdxEntry.ulSize);
	pMedia->ulSpIdxOffset += 4;
	MMPF_AVIPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &pMedia->SpIdxEntry.ulDuration);
	pMedia->ulSpIdxOffset += 4;
	
	if (nAdvance > 0) {
	    pMedia->ulSpIdxCount++;
	}
	if (nAdvance < 0) {
        pMedia->ulSpIdxCount--;
    }
    
	// Set index table offset
	MMPF_VIDPSR_SwapDepackCacheIdx(CacheType);
	
	pMedia->ulIdxOffset = (MMP_ULONG)pMedia->SpIdxEntry.ullOffset;
	if ((pMedia->ulIdxOffset + 32) > pMedia->ulFileSize) {
        return MMP_VIDPSR_ERR_EOS;
    }    
	pMedia->ulIdxOffset += 12;
	MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &pMedia->ulStdIdxNum);
	pMedia->ulIdxOffset += 4;
	pMedia->ulStdIdxCount = 0;
	pMedia->ulIdxOffset += 4; // skip ChunkID(4)
	
	MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &ulLowL);
	pMedia->ulIdxOffset += 4;
	MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &ulHighL);
	pMedia->ulIdxOffset += 4;
	pMedia->ullBaseOffset = ((MMP_ULONG64)ulHighL << 32) | ulLowL;
	pMedia->ulIdxOffset += 4; //Skip reserved(4)
	
	pMedia->ulReadCount = 32;
	if (nAdvance < 0) {
	    pMedia->ulIdxOffset += 8*(pMedia->ulStdIdxNum - 1);
	    pMedia->ulReadCount += 8*(pMedia->ulStdIdxNum - 1);
	    pMedia->ulStdIdxCount += pMedia->ulStdIdxNum - 1;
	}
	return err;
}

MMP_ERR MMPF_AVIPSR_GetNextIndex(MMP_AVI_MEDIA_INFO *pMedia, MMP_SHORT nAdvance, MMP_ULONG *ulPos, MMP_ULONG *ulSize)
{
	MMP_ULONG tag, flags, pos, len, index;
	MMP_ULONG index_pos;
	MMP_ERR err = MMP_ERR_NONE;
	MMP_ULONG ulStartIdx;
	MMP_UBYTE ubChar0, ubChar1;
	
	if(pMedia->bIsSuperIdx || pMedia->ulIdxOffset)
	{
    	if(pMedia->ulCodecType == CODEC_TYPE_VIDEO){
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_INDEX_CACHE);
    	    ulStartIdx = m_ulAVIVideoIdxPos[m_ubVideoCurTrack];
    	} else if(pMedia->ulCodecType == CODEC_TYPE_AUDIO){
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_AUDIO_INDEX_CACHE);
    	    ulStartIdx = m_ulAVIAudioIdxPos;
    	} else{
    	    return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    	}
	}
    if(pMedia->ulIdxOffset && !(pMedia->bIsSuperIdx))
    {
    	index_pos = pMedia->ulIdxOffset;
    	
    	for(;;){

    		if(lAVIIndexEnd != 0) {
        		if((index_pos >= lAVIIndexEnd || index_pos >= pMedia->ulFileSize)){
        		    return MMP_VIDPSR_ERR_EOS;
        		}
    		} else {
    		    if(index_pos >= pMedia->ulFileSize){
        		    return MMP_VIDPSR_ERR_EOS;
        		}
    		}
    		if(index_pos < ulStartIdx){
    		    return MMP_VIDPSR_ERR_EOS;
    		}
    		MMPF_AVIPSR_GetLittleLong(index_pos, 0, &tag);
    		MMPF_AVIPSR_GetLittleLong(index_pos + 4, 0, &flags);
    		MMPF_AVIPSR_GetLittleLong(index_pos + 8, 0, &pos);
    		MMPF_AVIPSR_GetLittleLong(index_pos + 12, 0, &len);
    		
    		if(nAdvance > 0){
    		    index_pos +=16;
    		}
    		
    		index = ((tag & 0xff) - '0') * 10;
    		index += ((tag >> 8) & 0xff) - '0';
    		ubChar0 = (tag >> 16) & 0xff;
    		ubChar1 = (tag >> 24) & 0xff;
    		
    		if(index == pMedia->ulTrackID && ((ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b')) || (ubChar0 == 'w' && ubChar1 == 'b'))){
    			pMedia->ulIdxOffset = index_pos;
    			if(ulPos != NULL) {
    			    *ulPos = pMedia->ulIndexMoviOffset + pos + 8;
    			}
    			if(ulSize != NULL) {
    			    *ulSize = len;
    			}
    	        if(lAVIIndexEnd != 0) {
    	            if(pMedia->ulIdxOffset >= lAVIIndexEnd || pMedia->ulIdxOffset >= pMedia->ulFileSize){
    	                return MMP_VIDPSR_ERR_EOS;
    	            }
    	        } else {
    	            if(pMedia->ulIdxOffset >= pMedia->ulFileSize){
    	                return MMP_VIDPSR_ERR_EOS;
    	            }
    	        }
    			return MMP_ERR_NONE;
    		}
    	}
	}
	else if(pMedia->bIsSuperIdx)
	{
	    if(((pMedia->ulReadCount >= pMedia->SpIdxEntry.ulSize) || (pMedia->ulStdIdxCount >= pMedia->ulStdIdxNum)) && (nAdvance > 0)){ // Read Next Super index table entry
	        err = MMPF_AVIPSR_GetNextSuperIdx(pMedia, nAdvance);
	        
	    }
	    
	    if(err == MMP_VIDPSR_ERR_EOS){
    	    return MMP_VIDPSR_ERR_EOS;
	    }
	    if(err != MMP_ERR_NONE) {
	        return err;
	    }
   
	    MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &pos);
	    MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset + 4, 0, &len);
		
		if(ulPos != NULL) {
		    len = len & AVI_SAMPLESIZEMASK;
		    *ulPos = (MMP_ULONG)pMedia->ullBaseOffset + pos;
		}
		if(ulSize != NULL) {
		    *ulSize = len;
		}
		
		if(nAdvance > 0){
            pMedia->ulIdxOffset += 8;
            pMedia->ulReadCount += 8;
            pMedia->ulStdIdxCount++;
	    } 
	} else {
	    return MMP_VIDPSR_ERR_EOS;
	}
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPF_AVIPSR_GetNextSample(MMP_AVI_MEDIA_INFO *pMedia, MMP_AVI_SAMPLE_INFO *pSample, MMP_SHORT nAdvance)
{
	MMP_ULONG tag, flags, pos, len, track_id;
	MMP_ULONG index_pos;
	MMP_ERR err = MMP_ERR_NONE;
	MMP_ULONG ulStartIdx;
	MMP_UBYTE ubChar0, ubChar1;
	MMP_ULONG ulUnknownChunk = 0, ulChunkCount = 0;
	#if (AIT_DIVXDRM_SUPPORT == 1)
	MMP_BOOL  bSampleReady = MMP_FALSE;
	#endif
	
    pMedia->bSeekEnable = bIsUnknownAviData;
	#if (SPEED_UP_AVI_OPEN)
	if((pMedia->bIsSuperIdx || pMedia->ulIdxOffset) && pMedia->bSeekEnable)
	#else
	if(pMedia->bIsSuperIdx || pMedia->ulIdxOffset)
	#endif
	{
    	if(pMedia->ulCodecType == CODEC_TYPE_VIDEO){
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_INDEX_CACHE);
    	    ulStartIdx = m_ulAVIVideoIdxPos[m_ubVideoDmxTrackNo];
    	} else if(pMedia->ulCodecType == CODEC_TYPE_AUDIO){
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_AUDIO_INDEX_CACHE);
    	    ulStartIdx = m_ulAVIAudioIdxPos;
    	} else{
    	    return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    	}
	} else {
	    ulStartIdx = pMedia->ulMoviOffset;
	    if(pMedia->ulCodecType == CODEC_TYPE_VIDEO){
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_CACHE);
    	} else if(pMedia->ulCodecType == CODEC_TYPE_AUDIO){
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_AUDIO_CACHE);
    	    
    	} else{
    	    return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    	}
	}
	
	#if (SPEED_UP_AVI_OPEN)
    if(pMedia->ulIdxOffset && pMedia->bSeekEnable && !(pMedia->bIsSuperIdx))
    #else
    if(pMedia->ulIdxOffset && !(pMedia->bIsSuperIdx))
    #endif
    {
    	index_pos = pMedia->ulIdxOffset;
    	
    	for(;;){
            
    		if(nAdvance < 0){
    		    index_pos -=16;
    		}

    		if(lAVIIndexEnd != 0) {
        		if((index_pos >= lAVIIndexEnd || index_pos >= pMedia->ulFileSize)){
        		    //DBG_S(3, "End index already\r\n");
        		    MEMSET0(pSample);
        		    return MMP_VIDPSR_ERR_EOS;
        		}
    		} else {
    		    if(index_pos >= pMedia->ulFileSize){
        		    //DBG_S(3, "End index already\r\n");
        		    MEMSET0(pSample);
        		    return MMP_VIDPSR_ERR_EOS;
        		}
    		}
    		if(index_pos < ulStartIdx){
    		    MEMSET0(pSample);
    		    return MMP_VIDPSR_ERR_EOS;
    		}
    		MMPF_AVIPSR_GetLittleLong(index_pos, 0, &tag);
    		//index_pos += 4;
    		MMPF_AVIPSR_GetLittleLong(index_pos + 4, 0, &flags);
    		//index_pos += 4;
    		MMPF_AVIPSR_GetLittleLong(index_pos + 8, 0, &pos);
    		//index_pos += 4;
    		MMPF_AVIPSR_GetLittleLong(index_pos + 12, 0, &len);
    		//index_pos += 4;
    		
    		if(nAdvance > 0){
    		    index_pos +=16;
    		}
    		
    		track_id = ((tag & 0xff) - '0') * 10;
    		track_id += ((tag >> 8) & 0xff) - '0';
    		ubChar0 = (tag >> 16) & 0xff;
    		ubChar1 = (tag >> 24) & 0xff;
    		
    		#if (SPEED_UP_AVI_SEEK)
    		if(pMedia->ulCodecType == CODEC_TYPE_VIDEO && m_bCountAudioChunk) {
                if(track_id == gAVIAudioMedia.ulTrackID && (ubChar0 == 'w' && ubChar1 == 'b')){
                    m_ulAudioChunkNum++;
                }
            }
            #endif
    		
    		if(track_id == pMedia->ulTrackID && ((ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b')) || (ubChar0 == 'w' && ubChar1 == 'b'))){
    			pMedia->ulIdxOffset = index_pos;
    			pSample->ulOffset = pMedia->ulIndexMoviOffset + pos + 8;
    			pSample->ulSize = len;
#if (SUPPORT_NO_IFLAG_SEEK == 1)                
                if(ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b')){
                    //printc(FG_YELLOW("Chk_0\r\n"));
                    MMPF_AVIPSR_ChkFrameIfHasIflag(pSample, &flags);
                }
#endif
    			pSample->ulFlag = flags;

    			if(nAdvance > 0) {
    			    pMedia->ulSamplePos = pSample->ulOffset + pSample->ulSize + (pSample->ulSize & 1);
    			} else {
    			    pMedia->ulSamplePos = pSample->ulOffset - 8;
    			}
    			if(nAdvance > 0){
        			if(pMedia->ulSampleSize == 0){
        	            pMedia->ulFrameIndex++;
        	        } else {
        	            pMedia->ulFrameIndex += (len / pMedia->ulSampleSize);
        	        }
    	        } else{
    	            if(pMedia->ulSampleSize == 0){
        	            pMedia->ulFrameIndex--;
        	        } else {
        	            pMedia->ulFrameIndex -= (len / pMedia->ulSampleSize);
        	        }
    	        }
    	        #if (AIT_DIVXDRM_SUPPORT == 1)
    	        if((nAdvance < 0) && DRM_DIVX_IsEnable()) {
    	            bSampleReady = MMP_TRUE;
    	            continue;
    	        }
    	        #endif
    	        
    	        if(lAVIIndexEnd != 0) {
    	            if(pMedia->ulIdxOffset >= lAVIIndexEnd || pMedia->ulIdxOffset >= pMedia->ulFileSize){
    	                return MMP_VIDPSR_ERR_EOS;
    	            }
    	        } else {
    	            if(pMedia->ulIdxOffset >= pMedia->ulFileSize){
    	                return MMP_VIDPSR_ERR_EOS;
    	            }
    	        }
    			return MMP_ERR_NONE;
    		}
    		#if (AIT_DIVXDRM_SUPPORT == 1)
    		else if((ubChar0 == 'd' && ubChar1 == 'd') && DRM_DIVX_IsEnable()){
                    MMP_UBYTE *p_key;   
                    MMP_ULONG ulPos = 0;
                    ulPos = pMedia->ulIndexMoviOffset + pos + 8;
                    if(pMedia->ulCodecType == CODEC_TYPE_VIDEO){ 
                        p_key = DRM_DIVX_GetBuffer(2, 0x10/*TBD*/); //Video key
                    }
                    else if (pMedia->ulCodecType == CODEC_TYPE_AUDIO){
                        p_key = DRM_DIVX_GetBuffer(1, 0x10/*TBD*/); //Audio key    
                    }
                    MMPF_VIDPSR_ReadFile(p_key, ulPos, len);
    		        if((nAdvance < 0) && bSampleReady) {
    		        	pMedia->ulIdxOffset = index_pos;
    		        	pMedia->ulSamplePos = ulPos - 8;
    		            break;
    		        }
    		}
    		#endif
    		
    	}
	}
	#if (SPEED_UP_AVI_OPEN)
	else if(pMedia->bIsSuperIdx && pMedia->bSeekEnable)
	#else
	else if(pMedia->bIsSuperIdx)
	#endif
	{
	    if(((pMedia->ulReadCount >= pMedia->SpIdxEntry.ulSize) || (pMedia->ulStdIdxCount >= pMedia->ulStdIdxNum)) && (nAdvance > 0)){ // Read Next Super index table entry
	        err = MMPF_AVIPSR_GetNextSuperIdx(pMedia, nAdvance);
	        
	    }
	    if(nAdvance < 0) {
	        pMedia->ulReadCount -= 8;
	        if(pMedia->ulReadCount < 32){
	            err = MMPF_AVIPSR_GetNextSuperIdx(pMedia, nAdvance);
	        } else {
	            pMedia->ulStdIdxCount--;
	            pMedia->ulIdxOffset -= 8;
	        }
	    }
	    if((pMedia->ulIdxOffset + 8) > pMedia->ulFileSize)
	        err = MMP_VIDPSR_ERR_EOS;
	    
	    if(err == MMP_VIDPSR_ERR_EOS){
	        MEMSET0(pSample);
    	    return MMP_VIDPSR_ERR_EOS;
	    }
	    if(err != MMP_ERR_NONE) {
	        MEMSET0(pSample);
	        return err;
	    }
   
	    MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &pos);
	    MMPF_AVIPSR_GetLittleLong(pMedia->ulIdxOffset + 4, 0, &len);
	    
	    pSample->ulOffset = (MMP_ULONG)pMedia->ullBaseOffset + pos;
		pSample->ulSize = len & AVI_SAMPLESIZEMASK;
		
		if(nAdvance > 0) {
		    pMedia->ulSamplePos = pSample->ulOffset + pSample->ulSize + (pSample->ulSize & 1);
		} else {
		    pMedia->ulSamplePos = pSample->ulOffset - 8;
		}
		
		if(len & AVI_ISKEYFRAME) {
		    pSample->ulFlag = 0;
		} else {
		    pSample->ulFlag = AVIIF_KEYFRAME;
		}
		if(nAdvance > 0){
    		if(pMedia->ulSampleSize == 0){
                pMedia->ulFrameIndex++;
            } else {
    	        pMedia->ulFrameIndex += (pSample->ulSize / pMedia->ulSampleSize);
            }
            pMedia->ulIdxOffset += 8;
            pMedia->ulReadCount += 8;
            pMedia->ulStdIdxCount++;
	    } else{
	        if(pMedia->ulSampleSize == 0){
    	        pMedia->ulFrameIndex--;
    	    } else {
    	        pMedia->ulFrameIndex -= (pSample->ulSize / pMedia->ulSampleSize);
    	    }
	    }
	} 
	else {
	    if(pMedia->ulSamplePos == 0)
	        pMedia->ulSamplePos = pMedia->ulMoviOffset + 4;
	    
	    for(;;){
	        index_pos = pMedia->ulSamplePos;

    		if(index_pos >= pMedia->ulFileSize){
		        MEMSET0(pSample);
    		    return MMP_VIDPSR_ERR_EOS;
		    }
    		
    		MMPF_AVIPSR_GetLittleLong(index_pos, 0, &tag);
    		MMPF_AVIPSR_GetLittleLong(index_pos + 4, 0, &len);
    		// LIST
    		
			if(tag == ATOM_LIST_BE || tag == ATOM_RIFF_BE) {
    		    index_pos += 8;
    		    MMPF_AVIPSR_GetLittleLong(index_pos, 0, &tag);
    		    index_pos += 4;
    		    pMedia->ulSamplePos = index_pos;
    		    if (tag == ATOM_MOVI_BE) {//if(tag == 0x69766F6D)
    		        pMedia->ulMoviEnd = len + (len & 1) + index_pos - 4;
    		    }
    		    continue;
    		}
    		index_pos += 8;
    		pMedia->ulSamplePos += len + (len & 1) + 8;
    		
    		#if KEEP_AVI_INDEX_POSTION
    		if(tag == 0) {
    		    MMP_ULONG ulSkipPos = 0;
    		    MMP_ULONG ulSkipSize = 0;
    		    MEMSET0(pSample);
    		    MMPF_AVIPSR_GetNextIndex(pMedia, 1, &ulSkipPos, &ulSkipSize);
    		    if(ulSkipPos != 0) {
    		        pSample->ulOffset = ulSkipPos;
    		        pSample->ulSize = ulSkipSize;
    		        if(pMedia->ulTrackID == 0) 
    		            VAR_L(0, pMedia->ulSamplePos);
    		        
    		        pMedia->ulSamplePos = ulSkipPos + ulSkipSize + (ulSkipSize & 1);
    		        if(pMedia->ulSampleSize == 0){
        	            pMedia->ulFrameIndex++;
        	        } else {
        	            pMedia->ulFrameIndex += (ulSkipSize / pMedia->ulSampleSize);
        	        }
    		        return MMP_ERR_NONE;
    		    }

    		    if(tag == 0 && (m_pBsBufInfo->ringData.Atoms < 1)) {
        		    MEMSET0(pSample);
        		    if(pMedia->bPreciseSeek) {
        		        pMedia->bPreciseSeek = MMP_FALSE;
        		    }
            		return MMP_VIDPSR_ERR_EOS;
        		}
    		    return MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA;
    		}
    		#endif
    		track_id = ((tag & 0xff) - '0') * 10;
    		track_id += ((tag >> 8) & 0xff) - '0';
    		ubChar0 = (tag >> 16) & 0xff;
    		ubChar1 = (tag >> 24) & 0xff;
            if (ubChar0 != 'w' && ubChar0 != 'd') {
                printc(FG_YELLOW("Unknown->tag:%x; len:%x\r\n"), tag, len);
                bIsUnknownAviData = MMP_TRUE;
            }
            else {
                bIsUnknownAviData = MMP_FALSE;                
            }
    		if(m_bCheckUnknownChunk) {
	    		if((ubChar0 == 'w' && ubChar1 == 'b') || (ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b'))) {
	    		} else {
	    		    ulUnknownChunk ++;
	    		    if (ulUnknownChunk > AVI_MAX_SEARCH_SAMPLE_COUNT) {
	    		        MEMSET0(pSample);
	    		        return MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA;
	    		    }
	    		}
			}
    		
            #if (SPEED_UP_AVI_SEEK)
            if(pMedia->ulCodecType == CODEC_TYPE_VIDEO && m_bCountPreAudio){
                if(track_id == gAVIAudioMedia.ulTrackID && (ubChar0 == 'w' && ubChar1 == 'b')) {
                    if(gAVIAudioMedia.ulSampleSize == 0)
                        m_ulPreAudioSize++;
                    else
                        m_ulPreAudioSize += len;
                } else if(track_id == pMedia->ulTrackID && (ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b'))){
                    m_bCountPreAudio = MMP_FALSE;
                }
            } else if(pMedia->ulCodecType == CODEC_TYPE_AUDIO && m_bCountPreVideo) {
                if(track_id == pMedia->ulTrackID && (ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b'))){
                    m_ulPreVideoChunk++;
                } else if(track_id == gAVIAudioMedia.ulTrackID && (ubChar0 == 'w' && ubChar1 == 'b')){
                    m_bCountPreVideo = MMP_FALSE;
                }
            }
            #endif
    		if(track_id == pMedia->ulTrackID && ((ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b')) || (ubChar0 == 'w' && ubChar1 == 'b'))){
    		    pSample->ulOffset = index_pos;
    		    pSample->ulSize = len;
//#if (SUPPORT_NO_IFLAG_SEEK == 1)                
//                if(ubChar0 == 'd' && (ubChar1 == 'c' || ubChar1 == 'b')){
//                    printc(FG_YELLOW("Chk_1\r\n"));
//                }
//#endif   		    
    		    #if KEEP_AVI_INDEX_POSTION
    		    	MMPF_AVIPSR_GetNextIndex(pMedia, 1, NULL, NULL);
    		    #endif
    		    
    		    if(pMedia->ulSampleSize == 0){
    	            pMedia->ulFrameIndex++;
    	        } else {
    	            pMedia->ulFrameIndex += (len / pMedia->ulSampleSize);
    	        }
    	        
	            if(pMedia->ulSamplePos >= pMedia->ulFileSize){
    	            DBG_S(3, "video Eos\r\n");
    	            return MMP_VIDPSR_ERR_EOS;
    	        }
                //printc(FG_GREEN("Hit->pos:%x; BSSize:%x (%d)\r\n"), index_pos, len, pMedia->ulFrameIndex);

     			return MMP_ERR_NONE;    
    		}
    		#if (AIT_DIVXDRM_SUPPORT == 1)
            else if((ubChar0 == 'd' && ubChar1 == 'd') && DRM_DIVX_IsEnable()){
                MMP_UBYTE *p_key;   
                if(pMedia->ulCodecType == CODEC_TYPE_VIDEO){ 
                    p_key = DRM_DIVX_GetBuffer(2, 0x10/*TBD*/); //Video key
                }
                else if (pMedia->ulCodecType == CODEC_TYPE_AUDIO){
                    p_key = DRM_DIVX_GetBuffer(1, 0x10/*TBD*/); //Audio key    
                }
                MMPF_VIDPSR_ReadFile(p_key, index_pos, len);
            }
    		#endif
    		else {
    		    ulChunkCount++;
    		    if(ulChunkCount > 60 && m_bCheckUnknownChunk == MMP_FALSE) {
    		        MEMSET0(pSample);
    		        if(index_pos >= pMedia->ulDataEnd) {
    		        	return MMP_VIDPSR_ERR_EOS;
    		        }
    		        return MMP_ERR_NONE;
    		    }
    		}
    		
        }
	}
	
	return MMP_ERR_NONE;
}

MMP_ERR MMPF_AVIPSR_SeekIFrame(const MMP_SHORT nAdvance, MMP_ULONG *requestFrame)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_AVI_SAMPLE_INFO TmpSample;
    MMP_AVI_MEDIA_INFO  TmpMedia;
    MMP_SHORT sDirect = 0;
    MMP_SHORT sSkipCount = 0;
    MMP_SHORT sHalfCount = 0;
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoDmxTrackNo];
    if(nAdvance > 0){
        sDirect = 1;
        sSkipCount = nAdvance;
    } else {
        sDirect = -1;
        sSkipCount = nAdvance * (-1);
    }
    sHalfCount = sSkipCount >> 1;
    if(pVidMedia->bIsSuperIdx || pVidMedia->ulIdxOffset){
        MEMCPY(&TmpMedia, &gAVIVideoMedia[m_ubVideoDmxTrackNo], sizeof(MMP_AVI_MEDIA_INFO));
        TmpSample.ulFlag = 0;

        while(sSkipCount){

            error = MMPF_AVIPSR_GetNextSample(&TmpMedia, &TmpSample, sDirect);

            if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS)){
                
                return error;
            }
            if(TmpSample.ulFlag & AVIIF_KEYFRAME){
                MEMCPY(&gAVIVideoMedia[m_ubVideoDmxTrackNo], &TmpMedia, sizeof(MMP_AVI_MEDIA_INFO));
                MEMCPY(&gAVIVideoSample[m_ubVideoDmxTrackNo], &TmpSample, sizeof(MMP_AVI_SAMPLE_INFO));
                sSkipCount--;
            }
            if(sSkipCount == sHalfCount) {
            }
            if(error == MMP_VIDPSR_ERR_EOS){
                break;
            }    
        }

        *requestFrame = pVidMedia->ulFrameIndex;
    }
    return error;
        
}

MMP_ULONG MMPF_AVIPSR_GetSampleNumByTime(MMP_ULONG ulTime, MMP_AVI_MEDIA_INFO *pMedia)
{  
    return (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)ulTime * (MMP_ULONG64)pMedia->ulRate)/(MMP_ULONG64)((MMP_ULONG64)pMedia->ulScale * (MMP_ULONG64)1000));
}

MMP_ERR MMPF_AVIPSR_SeekSampleNum(MMP_ULONG ulSampleNum, MMP_AVI_MEDIA_INFO *pMedia, MMP_AVI_SAMPLE_INFO *pSample, MMP_ULONG *ulKey)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_ULONG ulFrameCount = 0, ulPreFrameCount = 0;
    
    pMedia->ulSpIdxOffset -= pMedia->ulSpIdxCount*16;
    pMedia->ulSpIdxCount = 0;
    
    error = MMPF_AVIPSR_GetNextSuperIdx(pMedia, 1);
    if(error == MMP_ERR_NONE){
        ulPreFrameCount = ulFrameCount;
        ulFrameCount += pMedia->ulStdIdxNum;
        while(ulSampleNum >= ulFrameCount){
            error = MMPF_AVIPSR_GetNextSuperIdx(pMedia, 1);
            if(error != MMP_ERR_NONE){
                DBG_S(3, "error occur\r\n");
                if(error == MMP_VIDPSR_ERR_EOS) {
                    return MMPF_AVIPSR_SeekSampleNum(ulFrameCount - 1, pMedia, pSample, ulKey);
                }
                return error;
            }
            ulPreFrameCount = ulFrameCount;
            ulFrameCount += pMedia->ulStdIdxNum;
        }
        pMedia->ulFrameIndex = ulPreFrameCount;
        pMedia->ulSamplePos = 0;
        #if SPEED_UP_AVI_SPIDX_SEEK
        if((MMP_LONG)(ulSampleNum - pMedia->ulFrameIndex) > 150) {
            MMP_ULONG ulSkip = ulSampleNum - pMedia->ulFrameIndex - 150;
            pMedia->ulFrameIndex += ulSkip;
            pMedia->ulIdxOffset += 8*ulSkip;
            pMedia->ulReadCount += 8*ulSkip;
            pMedia->ulStdIdxCount += ulSkip;
        }
        #endif
        while(ulSampleNum > pMedia->ulFrameIndex) {
            error = MMPF_AVIPSR_GetNextSample(pMedia, pSample, 1);
            if(error != MMP_ERR_NONE){
                DBG_S(0, "MMPF_AVIPSR_GetNextSample error occur\r\n");
                return error;
            }
            if(pSample->ulFlag & AVIIF_KEYFRAME){
                *ulKey = pMedia->ulFrameIndex;
            }
        }
        
    } else {
        DBG_S(0, "MMPF_AVIPSR_SeekSampleNum error\r\n");
    }
    return error;
}

MMP_ERR MMPF_AVIPSR_SeekVIDSample(MMP_ULONG ulSampleNum, MMP_ULONG *ulKeyNum)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoDmxTrackNo];
    MMP_AVI_SAMPLE_INFO *pVidSample = &gAVIVideoSample[m_ubVideoCurTrack];     
 	if(pVidMedia->ulIdxOffset && !(pVidMedia->bIsSuperIdx)) {
    	pVidMedia->ulFrameIndex = 0;
	    pVidMedia->ulIdxOffset = m_ulAVIVideoIdxPos[m_ubVideoCurTrack];
    
    	gAVIAudioMedia.ulSamplePos = 0;
    
	    while(ulSampleNum >= pVidMedia->ulFrameIndex){

	        error = MMPF_AVIPSR_GetNextSample(pVidMedia, pVidSample, 1);

    	    if(error != MMP_ERR_NONE){
        	    DBG_S(3, "error occur\r\n");
            	return error;
	        }
        
    	    if(pVidSample->ulFlag & AVIIF_KEYFRAME){
        	    *ulKeyNum = pVidMedia->ulFrameIndex;
	        }
    	}
	} 
	else if(pVidMedia->bIsSuperIdx) {
    	error = MMPF_AVIPSR_SeekSampleNum(ulSampleNum, pVidMedia, pVidSample, ulKeyNum);
	} 
	else {
//	    MMP_AVI_SAMPLE_INFO TmpSample;
//	    MMP_AVI_MEDIA_INFO  TmpMedia;
//	    MMP_UBYTE           ubBuf[5];
//	    MMP_ULONG           ulOffset;
//    
//	    pVidMedia->ulFrameIndex = 0;
//	    pVidMedia->ulSamplePos = 0;
//    
//	    MEMCPY(&TmpMedia, &pVidMedia, sizeof(MMP_AVI_MEDIA_INFO));
//	    MEMSET0(&TmpSample);
//    
//	    while(ulSampleNum != TmpMedia.ulFrameIndex){
//	        //VAR_L(3, TmpMedia.ulFrameIndex);
//	        error = MMPF_AVIPSR_GetNextSample(&TmpMedia, &TmpSample, 1);
//	        if(error != MMP_ERR_NONE){
//	            DBG_S(3, "error occur 3\r\n");
//	            return error;
//	        }
//	        MMPF_VIDPSR_ReadFile(ubBuf, TmpSample.ulOffset, 5);
//        
//	        // if != 0x000001b6 or bit[7:6] != 0, assume it is I frame
//	        if((ubBuf[0] != 0) || (ubBuf[1] != 0) || ubBuf[2] != 0x01 || ubBuf[3] != 0xb6 || (ubBuf[4] & 0xC0) == 0){
//	            DBG_S(3, "Find Key Frame\r\n");
//         
//	            ulOffset = TmpMedia.ulSamplePos;
//	            TmpMedia.ulSamplePos -= TmpSample.ulSize + (TmpSample.ulSize & 0x01) + 8;
//	            MEMCPY(&pVidMedia, &TmpMedia, sizeof(MMP_AVI_MEDIA_INFO));
//	            MEMCPY(&gAVIVideoSample, &TmpSample, sizeof(MMP_AVI_SAMPLE_INFO));
//	            TmpMedia.ulSamplePos = ulOffset;
//	        }
//	    }
    	return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    }
    return error;
}

MMP_ERR MMPF_AVIPSR_SeekAUDSample(MMP_ULONG ulSampleNum, MMP_ULONG ulEntries, MMP_ULONG ulFrameNum)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_AVI_MEDIA_INFO TempMedia;
    m_ulPreAudioOffset      = 0;
    m_ulPreAudioReduntSize  = 0;
    m_ulBuffedAudioAtoms  = 0;
  
  #if (SPEED_UP_AVI_OPEN)
  if(gAVIAudioMedia.bSeekEnable) {
      if(gAVIAudioMedia.ulIdxOffset && !(gAVIAudioMedia.bIsSuperIdx)){
        gAVIAudioMedia.ulFrameIndex = 0;
        gAVIAudioMedia.ulIdxOffset = m_ulAVIAudioIdxPos;
        gAVIAudioMedia.ulSamplePos = 0;
      } 
      else if(gAVIAudioMedia.bIsSuperIdx) {
        MMP_ULONG ulFrameCount = 0, ulPreFrameCount = 0;
        gAVIAudioMedia.ulFrameIndex = ulFrameNum;
        gAVIAudioMedia.ulSpIdxOffset -= gAVIAudioMedia.ulSpIdxCount*16;
        gAVIAudioMedia.ulSpIdxCount = 0;
        gAVIAudioMedia.ulSamplePos = 0;
        
        error = MMPF_AVIPSR_GetNextSuperIdx(&gAVIAudioMedia, 1);
        if(error == MMP_ERR_NONE){
            ulFrameCount += gAVIAudioMedia.ulStdIdxNum;
            while(ulEntries >= ulFrameCount){
                error = MMPF_AVIPSR_GetNextSuperIdx(&gAVIAudioMedia, 1);
                if(error != MMP_ERR_NONE){
                    return error;
                }
                ulPreFrameCount = ulFrameCount;
                ulFrameCount += gAVIAudioMedia.ulStdIdxNum;
            }
            {
                MMP_ULONG ulSkip = ulEntries - ulPreFrameCount;
                gAVIAudioMedia.ulIdxOffset += 8*ulSkip;
                gAVIAudioMedia.ulReadCount += 8*ulSkip;
                gAVIAudioMedia.ulStdIdxCount += ulSkip;
            }
            MMPF_AVIPSR_GetNextSample(&gAVIAudioMedia, &gAVIAudioSample, 1);
            MMPF_AVIPSR_GetNextSample(&gAVIAudioMedia, &gAVIAudioSample, -1);
        } else {
            return error;
        }
      }
  }
  gAVIAudioMedia.ulFrameIndex = ulFrameNum;
  
  #else
  if(gAVIAudioMedia.ulIdxOffset && !(gAVIAudioMedia.bIsSuperIdx)){
    gAVIAudioMedia.ulFrameIndex = 0;
    gAVIAudioMedia.ulIdxOffset = m_ulAVIAudioIdxPos;
  } 
  else if(gAVIAudioMedia.bIsSuperIdx) {
    gAVIAudioMedia.ulFrameIndex = 0;
    gAVIAudioMedia.ulSpIdxOffset -= gAVIAudioMedia.ulSpIdxCount*16;
    gAVIAudioMedia.ulSpIdxCount = 0;
    MMPF_AVIPSR_GetNextSuperIdx(&gAVIAudioMedia, 1);   
  }
  else {
    gAVIAudioMedia.ulFrameIndex = 0;
    gAVIAudioMedia.ulSamplePos = 0;
  }
  #endif
    memcpy(&TempMedia, &gAVIAudioMedia, sizeof(gAVIAudioMedia));
    while(ulSampleNum > gAVIAudioMedia.ulFrameIndex){
        memcpy(&TempMedia, &gAVIAudioMedia, sizeof(gAVIAudioMedia));
        error = MMPF_AVIPSR_GetNextSample(&gAVIAudioMedia, &gAVIAudioSample, 1);
        if(error != MMP_ERR_NONE)
            return error;
    }
    memcpy(&gAVIAudioMedia, &TempMedia, sizeof(gAVIAudioMedia));
    return error;
}

MMP_ERR MMPF_AVIPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoDmxTrackNo];
    MMP_AVI_SAMPLE_INFO *pVidSample = &gAVIVideoSample[m_ubVideoCurTrack];    
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
    *RequestFrame = 0;
    
    ///- Get video presentation time offset for following decoded bitstream
    /// See PTS/DTS in MPEG for further study.
    if (pSeek->mode == MMPF_M_SEEK_NORMAL) {
        error = MMPF_AVIPSR_GetNextSample(pVidMedia, pVidSample, 1);
    }
    else if (pSeek->mode == MMPF_M_FF_KEY) {
        #if (SPEED_UP_AVI_OPEN)
        if(pVidMedia->ulIdxOffset == 0 && !(pVidMedia->bIsSuperIdx)) {
            MMPF_AVIPSR_SetIndex(pVidMedia->ulMoviOffset, pVidMedia->ulMoviEnd, gVidFileInfo.file_size, pVidMedia->ulTrackID);
        }
        pVidMedia->bSeekEnable = MMP_TRUE;
        #endif
        #if (SPEED_UP_AVI_SEEK) && (VIDEO_BGPARSER_EN == 0)
        {
            MMP_BOOL bNoIndex = MMP_FALSE;
            error = MMPF_IDXPSR_SeekKeySample(pSeek->sSpeed/100, &bNoIndex, &pVidMedia->ulIdxOffset, &pVidMedia->ulFrameIndex);
            if(bNoIndex) {
                error = MMPF_AVIPSR_SeekIFrame(pSeek->sSpeed/100, RequestFrame);
            } else {
                #if (AIT_DIVXDRM_SUPPORT == 1)
                if(DRM_DIVX_IsEnable()) {
                    pVidMedia->ulIdxOffset -= 16;
                }
                #endif
                MMPF_AVIPSR_SeekIFrame(1, RequestFrame);
            }
        }
        #else
        error = MMPF_AVIPSR_SeekIFrame(pSeek->sSpeed/100, RequestFrame);
        #endif
        #if (SPEED_UP_AVI_OPEN)
        pVidMedia->bSeekEnable = MMP_FALSE;
        #endif
        if (error == MMP_VIDPSR_ERR_EOS) {
			// Will 20090813
//            error = MMP_ERR_NONE; // continue to play so not return EOS
            pSeek->mode = MMPF_M_SEEK_NONE;
        }

    }
    else if (pSeek->mode == MMPF_M_SEEK_NONE) {
        return MMP_VIDPSR_ERR_EOS;
    }
    else {
        return MMP_VIDPSR_ERR_INCORRECT_STATE_OPERATION;
    }
#if 0    
	error = MMPF_AVIPSR_GetNextSample(&pVidMedia, &pVidSample, 1);
#endif
	//gVideoSample.ulOffset = gAVIVideoSample.ulOffset;
	//gVideoSample.ulSize = gAVIVideoSample.ulSize;
	//gVideoSample.lSampleTime = gAVIVideoSample.lSampleTime;
	
    //#if (CHIP == MCR_V2)
	// calculate the offsets before and after the bitstream in all cases
	m_3GPPdata.usBSOffset = sizeof(MMPF_M_FRAME_INFO);
    if (m_ulCodecType == MMP_VID_VIDEO_MJPG) {
        m_3GPPdata.usBSOffset = ALIGN256(m_3GPPdata.usBSOffset);
    }
    // ARM address padding
    m_3GPPdata.usEOFPadding = (4 - ((pVidSample->ulSize + m_3GPPdata.usBSOffset) % 4)) % 4 + RING_BUF_TAILING;
	//#endif
    
    if (m_bBsSupportLargeFrame) {
        // if it's a large frame, use fixed header 0x20;
        if ((pVidSample->ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding) > m_ulVideoStreamLen){
            m_3GPPdata.usBSOffset = 0x20;
        }
    }
    
#if (RING_BUF_TAILING % 4) != 0
    #error RING_BUF_TAILING must be a multiple of 4
#endif
	return error;	
}

MMP_ULONG MMPF_AVIPSR_GetTime(MMP_AVI_MEDIA_INFO *pMedia)
{
    MMP_ULONG time;
    time = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)pMedia->ulFrameIndex * (MMP_ULONG64)pMedia->ulScale * (MMP_ULONG64)1000) / pMedia->ulRate);
    return time;
}

MMP_ULONG MMPF_AVIPSR_CalculateTime(MMP_AVI_MEDIA_INFO *pMedia, MMP_ULONG ulCount)
{
    MMP_ULONG time;
    time = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)ulCount * (MMP_ULONG64)pMedia->ulScale * (MMP_ULONG64)1000) / pMedia->ulRate);
    return time;
}

MMP_ERR MMPF_AVIPSR_GetSingleFrame( MMP_ULONG ulDestAddr, MMP_ULONG ulSizeToLoad,
                                    MMP_ULONG ulSeg1, MMP_ULONG ulSeg2,
                                    MMP_ULONG *pulContainerOffset)
{
    MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   ulRingTailing = 0;
    MMP_UBYTE   *buf_start = m_VideoStreamBuf;
    MMP_AVI_SAMPLE_INFO *pVidSample = &gAVIVideoSample[m_ubVideoDmxTrackNo];
    // Swap cache index to video data
    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_CACHE);
    // Need wrap
    if (ulSizeToLoad > ulSeg1) {

        if (MMPF_VIDPSR_ReadFile((MMP_UBYTE*)ulDestAddr, *pulContainerOffset, ulSeg1) != MMP_ERR_NONE)
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;

        if (MMPF_VIDPSR_ReadFile(buf_start, pVidSample->ulOffset + ulSeg1, ulSizeToLoad - ulSeg1) != MMP_ERR_NONE)
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;

        #if (RING_BUF_TAILING > 0)
        MMPF_BS_AccessVideoHeader(m_pBsBufInfo->buf + ulSizeToLoad - ulSeg1, (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
        #endif
    }
    else {

        if (MMPF_VIDPSR_ReadFile((MMP_UBYTE*)ulDestAddr, *pulContainerOffset, ulSizeToLoad) != MMP_ERR_NONE)
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;

        if (m_bBsSupportLargeFrame) {
            *pulContainerOffset += ulSizeToLoad;

            #if (RING_BUF_TAILING > 0)
         	if (!m_3GPPdata.bLargeFrame) {
                MMPF_BS_AccessVideoHeader((MMP_UBYTE*)(ulDestAddr + ulSizeToLoad),
                                         (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
      		}
            #endif
        }
    }

    return error;
}

/** @brief Load audio data according the the 3GP atoms

Basically it loads one audio frame. But some times if it reaches the limited size, 
it loads partial data and require call twice to load one audio frame.
@param[in] dataptr The address to be loaded
@param[in] Size The size to be loaded
@param[out] LoadedSize The actual loaded size
@return The status of the function call
*/ // using audio frame header uses extra 140 bytes of code
MMP_ERR MMPF_AVIPSR_LoadAudioData(MMP_UBYTE *dataptr, MMP_ULONG Size, MMP_ULONG *LoadedSize)
{
    typedef struct TEMP_AUDIO_HEADER {
        MMP_ULONG ulTime;
        MMP_ULONG ulSize;
        MMP_USHORT ulFlag;
        MMP_USHORT ulFlag2;
    }TEMP_AUDIO_HEADER;
    
    MMP_ULONG                   LoadSize;
    MMP_AVI_SAMPLE_INFO         tmpAtom;
    MMP_USHORT                  usHeaderSizeToWrite = 0;
    MMP_ERR                     error = MMP_ERR_NONE;
    static MMP_USHORT 			usHeadherReduntSize = 0;//How many bytes left last time
    static TEMP_AUDIO_HEADER 	curHeader;
    
    #ifndef SIZEOF_AUDIO_HEADER
    #define SIZEOF_AUDIO_HEADER (12)
    #endif

    if ( (m_ulPreAudioReduntSize == 0) && (m_bAudioEOS) ) {
        *LoadedSize = 0;
        return MMP_ERR_NONE;
    }
    
    LoadSize = 0;

    if (m_ulPreAudioReduntSize == 0) {
        /// Change for 3gp atom: moov\\trak (soun)
        
        curHeader.ulTime = MMPF_AVIPSR_GetTime(&gAVIAudioMedia);
        
        error = MMPF_AVIPSR_GetNextSample(&gAVIAudioMedia, &tmpAtom, 1);

        if(error == MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA) {
            *LoadedSize = 0;
            m_ulDemuxErrorCount++;
            return MMP_ERR_NONE;
        }
        
        if(m_ulAudioJumpTime != 0) {
            MMP_ULONG ulJumpSize;
            ulJumpSize = (gVidFileInfo.audio_info.bitrate * m_ulAudioJumpTime) / 8000;
            if(gAVIAudioMedia.ulSampleSize) {
                if((ulJumpSize % gAVIAudioMedia.ulSampleSize)!=0) {
                    ulJumpSize = ulJumpSize + (gAVIAudioMedia.ulSampleSize - (ulJumpSize % gAVIAudioMedia.ulSampleSize));
                }
            } else {
                ulJumpSize = ALIGN4(ulJumpSize);
            }
            if(ulJumpSize <= tmpAtom.ulSize){
                tmpAtom.ulOffset += ulJumpSize;
                tmpAtom.ulSize -= ulJumpSize;
            }
            curHeader.ulTime += m_ulAudioJumpTime;
            m_ulAudioJumpTime = 0;
        }
        
        usHeadherReduntSize = SIZEOF_AUDIO_HEADER;

        m_ulPreAudioReduntSize = tmpAtom.ulSize + SIZEOF_AUDIO_HEADER;
        
        #if (AIT_DIVXDRM_SUPPORT == 1)
        m_ulPreAudioChunkSize = tmpAtom.ulSize;
        #endif
        
        curHeader.ulSize = m_ulPreAudioReduntSize;

        curHeader.ulFlag2 ++;
        curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;
    
        m_ulPreAudioOffset = tmpAtom.ulOffset;
        //gl3gpAudioSize -= tmpAtom.ulSize;
        
        curHeader.ulFlag = BUFFERFLAG_EOF;
        if (error == MMP_VIDPSR_ERR_EOS) {
            curHeader.ulFlag |= BUFFERFLAG_EOS;
            m_bAudioEOS = MMP_TRUE;
        }
        m_ulBuffedAudioAtoms++;
    }

    if (m_ulPreAudioReduntSize) {

        // write header
        if (usHeadherReduntSize > 0) { // there is header to write
            MMP_USHORT i;
            if (usHeadherReduntSize >= Size) {
                usHeaderSizeToWrite = Size;
            }
            else {
                usHeaderSizeToWrite = usHeadherReduntSize;
            }
            m_ulPreAudioReduntSize -= usHeaderSizeToWrite;
#if 0
            memcpy((MMP_UBYTE*)(dataptr), (MMP_UBYTE*)((MMP_UBYTE*)(&curHeader)+(SIZEOF_AUDIO_HEADER - usHeadherReduntSize)), usHeaderSizeToWrite);
            dataptr+=usHeaderSizeToWrite;
#else
            for (i = SIZEOF_AUDIO_HEADER - usHeadherReduntSize;
                 i < SIZEOF_AUDIO_HEADER - usHeadherReduntSize + usHeaderSizeToWrite; i++) {
                *(dataptr ++) = *(((MMP_UBYTE*)(&curHeader)) + i);
            }
#endif                 
            usHeadherReduntSize -= usHeaderSizeToWrite;
        }
        Size -= usHeaderSizeToWrite;

        if (m_ulPreAudioReduntSize >= Size) {
            LoadSize = Size;
        }
        else {
            LoadSize = m_ulPreAudioReduntSize;
        }
        m_ulPreAudioReduntSize -= LoadSize;

        MMPF_VIDPSR_SwapDepackCacheIdx(AVI_AUDIO_CACHE);
#if (AIT_DIVXDRM_SUPPORT == 1)
        if(DRM_DIVX_IsEnable() && m_ulPreAudioChunkSize <= AUDIO_TEMP_BS_SIZE) {
            if(m_ulPreAudioChunkSize != 0) {
                if ( MMPF_VIDPSR_ReadFile((MMP_UBYTE *)m_ulAudioTempBuf, tmpAtom.ulOffset, m_ulPreAudioChunkSize) != MMP_ERR_NONE) {
                    error = MMP_VIDPSR_ERR_CONTENT_CORRUPT;
                }
                DRM_DIVX_DecryptAudio((MMP_UBYTE *)m_ulAudioTempBuf, m_ulPreAudioChunkSize);
                m_ulPreAudioChunkSize = 0;
                m_ulPreAudioOffset = m_ulAudioTempBuf;
            }
            memcpy(dataptr, (MMP_UBYTE *)m_ulPreAudioOffset, LoadSize);
        } else
#endif
        {

        if ( MMPF_VIDPSR_ReadFile(dataptr, m_ulPreAudioOffset, LoadSize) != MMP_ERR_NONE) 
            error = MMP_VIDPSR_ERR_CONTENT_CORRUPT;
        }
        m_ulPreAudioOffset += LoadSize;

        *LoadedSize = LoadSize + usHeaderSizeToWrite;
    }
    else {
        *LoadedSize = 0;
    }
    return error;
}

/** @brief Fill the audio buffer without being wrapped

@param[in] ulSize The size to be filled in the audio buffer.
@param[in,out] usTransferSize The size to be transfered. It would be accumulated so that it should be
                              initialized well.
@note This function works with audio pointers.
@return Always returns success.
*/
static MMP_ERR MMPF_AVIPSR_FillAudioBuffer(MMP_ULONG ulDestAddr,MMP_ULONG ulSize, MMP_USHORT *usXferSize)
{
    MMP_ULONG   ulFileReturnSize2 = 1;
    MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   ulLoop = 0;
    
    *usXferSize = 0;

    while (ulSize > *usXferSize && ulFileReturnSize2 != 0) {
#if 0        
        error = MMPF_3GPPSR_LoadAudioData((MMP_UBYTE *)(ulDestAddr+ *usTransferSize),
                                  		  ulSize - *usTransferSize, &ulFileReturnSize2);
       	
        if (error == MMP_ERR_NONE)
            *usTransferSize += ulFileReturnSize2;
        else
            return error;
    }    
#else        
        ulLoop++;
        if(ulLoop > 35)
            break;
        error = MMPF_AVIPSR_LoadAudioData((MMP_UBYTE *)(ulDestAddr+ *usXferSize),
                                  ulSize - *usXferSize, &ulFileReturnSize2);
        *usXferSize += ulFileReturnSize2;
        if (error != MMP_ERR_NONE && error != MMP_VIDPSR_ERR_EOS) {
            return error;
        }
    }
#endif    
    return error;
}

#if 0
void _____AVIBuildIndexTable_APIs_____(){}
#endif

MMP_ERR MMPF_IDXPSR_SetCacheBuf(MMP_ULONG ulCacheAddr, MMP_ULONG ulCacheSize)
{
    if (ulCacheSize % 2) {
        return MMP_AUDIO_ERR_PARAMETER;
    }
    m_ulIDXPSRCacheAddr = ulCacheAddr;
    m_ulIDXPSRCacheSize = ulCacheSize;

    return MMP_ERR_NONE;
}

MMP_ERR MMPF_IDXPSR_Initialize(MMP_ULONG ulFileID, MMP_ULONG ulFileSize)
{ 
	MMP_ERR	err = MMP_ERR_NONE;
	
    m_ulIDXPSRfileID = ulFileID;
    m_ulIDXPSRfileSize = ulFileSize;
	m_ulIDXPSRPreSeekPos = 0xFFFFFFFF;

	return err;
}

/** @brief Read data from an open file.

This function implements how the cache uses the file system to load its data
@param[in]  fileHandle   file id for reading
@param[in]  pData        Pointer to a buffer to receive the data
@param[in]  SeekPos      Offset to seek
@param[in]  NumBytes     Number of bytes to be read
@param[out] pReadCount   Number of bytes read.
@retval MMP_ERR_NONE Success
@retval MMP_M_ERR_RESOURCES_PREEMPTED The file is not be able to read. It might be caused by removing the
        media/card during playing
@retval MMP_VIDPSR_ERR_INVALID_STATE The data server is not assigned
@retval MMP_VIDPSR_ERR_PARAMETER Invalid seeking address
*/
MMP_ERR MMPF_IDXPSR_ReadSource(void* fileHandle, void *pData, const MMP_ULONG64 SeekPos,
                                MMP_ULONG NumBytes, MMP_ULONG *pReadCount)
{
    MMP_ERR     error;
    #if (VIDEO_HIGHPRIORITY_FS == 1)
    m_BGPSR_pFSHandler = fileHandle;
    m_BGPSR_pFSData = pData;
    m_BGPSR_ulSeekPos = SeekPos;
    m_BGPSR_ulNumBytes = NumBytes;
    m_BGPSR_pulReadCount = pReadCount;
    MMPF_OS_ReleaseSem(gBGFSStart_SemID);
    MMPF_OS_AcquireSem(gBGWaitFS_SemID, 0);
    error = m_BGPSR_Error;
    if ((MMP_ERR_NONE != error) && (*pReadCount == 0)) {
        DBG_S(0, "MMPF_IDXPSR_ReadSource error!!!!\r\n");
        return MMP_VIDPSR_ERR_RESOURCES_PREEMPTED;
    }
    return MMP_ERR_NONE;
    #else
    *pReadCount = NumBytes;

    #if (VIDEO_NONPREEMPT_FS == 1)
    MMPF_OS_AcquireSem(gFSProtect_SemID, 0);
    #endif
    if (MMPF_FS_FSeek((MMP_ULONG)fileHandle, (MMP_LONG64)SeekPos, MMP_FS_FILE_BEGIN)) {
        return MMP_VIDPSR_ERR_PARAMETER;
    }

    error = MMPF_FS_FRead((MMP_ULONG)fileHandle, (MMP_UBYTE*)pData, NumBytes, pReadCount);
    #if (VIDEO_NONPREEMPT_FS == 1)
    MMPF_OS_ReleaseSem(gFSProtect_SemID);
    #endif
    
    if ((MMP_ERR_NONE != error) && (*pReadCount == 0)) {
        return MMP_VIDPSR_ERR_RESOURCES_PREEMPTED;
    }
    return MMP_ERR_NONE;
    #endif
}

/** @brief Reload AV depack cache

This is used to reload the depack cache when the cache is missed.
@param[in] SeekPos Position in the 3gp file.
@pre The index is swapped to the cache index
*/
static MMP_ERR MMPF_IDXPSR_ReloadCache(MMP_ULONG ulSeekpos) 
{
    MMP_ULONG   readsize = 0;
    MMP_ERR     ret = MMP_ERR_NONE;

    ret = MMPF_IDXPSR_ReadSource((void*)m_ulIDXPSRfileID, (void*)m_ulIDXPSRCacheAddr,
                                   ulSeekpos, m_ulIDXPSRCacheSize, &readsize);

    m_ulIDXPSRPreSeekPos = ulSeekpos;
   
    return  ret;
}
/** @brief Read the data from the depack cache

Abstract Layer for 3gp player to access audio/video/meta data
@param[in] SeekPos The offset in the video file.
@param[in] size The size to be read.
@param[out] target The target address to put the data.
*/
static MMP_ERR MMPF_IDXPSR_ReadCache(MMP_UBYTE *ubTarget, MMP_ULONG ulSeekpos, MMP_ULONG ulReadsize)
{
    MMP_UBYTE *source;

    source = (MMP_UBYTE *)m_ulIDXPSRCacheAddr + (ulSeekpos & (m_ulIDXPSRCacheSize - 1));
    while (ulReadsize > 0) {
        *ubTarget++ = *source++;
        ulReadsize--;
    }
    
    return MMP_ERR_NONE;
}

/** @brief Read the 3GP file via depack cache buffers

@param[in] ptr Address to put the read data
@param[in] SeekPos Offset in the video file.
@param[in] size Size to read
@param[in] padding_size Size to padding
@note Currently this function is not thread safe.
@pre The caller should have already align the address with 8 bytes.
@pre The caller should swap the cache index
@return The status of the function call. MMP_ERR_NONE for success.
*/
MMP_ERR MMPF_IDXPSR_ReadFile(MMP_UBYTE *ubTarget, MMP_ULONG ulSeekpos, MMP_ULONG ulReadsize)
{
    MMP_ULONG   readed_size, canread_size, rem_size;
    MMP_ULONG64 cur_seekpos;
    MMP_ERR     ret;

    cur_seekpos = ulSeekpos;
    readed_size = 0;
    rem_size    = ulReadsize;

    while (readed_size < ulReadsize) {
        if (((m_ulIDXPSRPreSeekPos) & ~(m_ulIDXPSRCacheSize - 1)) != (cur_seekpos & ~(m_ulIDXPSRCacheSize - 1))) {
            // cache miss
            ret = MMPF_IDXPSR_ReloadCache(cur_seekpos & ~(m_ulIDXPSRCacheSize - 1));

		    if ((ret != MMP_ERR_NONE)) {
    			return ret;
    		}
        }

        canread_size = m_ulIDXPSRCacheSize - (cur_seekpos & (m_ulIDXPSRCacheSize - 1));
        if (rem_size > canread_size) {
            MMPF_IDXPSR_ReadCache(ubTarget, cur_seekpos, canread_size);
            rem_size    -= canread_size;
            readed_size += canread_size;
            cur_seekpos += canread_size;
            ubTarget    += canread_size;
            
            if (rem_size >= m_ulIDXPSRCacheSize) {
                ret = MMPF_IDXPSR_ReadSource((void*)m_ulIDXPSRfileID, 
                                          ubTarget, 
                                          cur_seekpos,
                                          rem_size & (~(m_ulIDXPSRCacheSize - 1)), 
                                          &canread_size);
                
                if ((ret != MMP_ERR_NONE) && (ret != MMP_FS_ERR_EOF)) {
    			    return ret;
    			}
    			    
                rem_size                    -= canread_size;
                readed_size                 += canread_size;
                cur_seekpos                 += canread_size;
                ubTarget                    += canread_size;
                m_ulIDXPSRPreSeekPos       = cur_seekpos - m_ulIDXPSRCacheSize;
            }
        }
        else {
            MMPF_IDXPSR_ReadCache(ubTarget, cur_seekpos, rem_size);
            readed_size += rem_size;
        }
    }

    return MMP_ERR_NONE;
}

static MMP_ULONG MMPF_IDXPSR_GetLittleLong(const MMP_ULONG ulcurSeek, MMP_SHORT advance, MMP_ULONG *value)
{
	MMP_UBYTE buf[4];

	MMPF_IDXPSR_ReadFile(buf, ulcurSeek, 4);
	
	*value = (((unsigned int) buf[3]) << 24) + (((unsigned int) buf[2]) << 16) + (((unsigned int) buf[1]) << 8) + 
           (unsigned int) buf[0];
	return *value;
}

MMP_IDX_AVIMEDIA_INFO* MMPF_IDXPSR_GetConText(void)
{
    return &m_IndexContext;
}

MMP_ULONG MMPF_IDXPSR_ParseIndexTable(MMP_IDX_AVIMEDIA_INFO *pCtxt, MMP_ULONG ulIdx, MMP_ULONG *ulVideoEnd, MMP_ULONG *ulAudioEnd)
{
	MMP_ULONG tag, size;
	MMP_ULONG nb_entries, i;
	MMP_ULONG tag1, flags, len, track_id, pos;
	MMP_LONG index_pos;
	MMP_ULONG ulTime = 0;
	MMP_ULONG ulLastKeyOffset = 0;
	MMP_ULONG ulLastKeyFrameNum = 0;
	MMP_ULONG ulCurrentFrameNum = 0;
	MMP_IDX_AVITRACK_INFO *pVidCtxTrack = NULL;
#if (SUPPORT_NO_IFLAG_SEEK == 1) 	
    MMP_AVI_SAMPLE_INFO AVIVideoSampleTmp;     
    MMP_AVI_SAMPLE_INFO *pVidSample = &AVIVideoSampleTmp;    
    MMP_ERR err = MMP_ERR_NONE;    
#endif    
	pCtxt->ulCurrentOffset = ulIdx;
	pVidCtxTrack = (MMP_IDX_AVITRACK_INFO *)&pCtxt->VideoTrack[m_ubVideoCurTrack];
	
	for(;;){
	    #if (VIDEO_BGPARSER_EN == 1)
	    if(m_bCloseBGParser)
	        break;
	    #endif

        if(pCtxt->ulCurrentOffset >= gAVIVideoMedia[m_ubVideoCurTrack].ulFileSize) 
            goto the_end;

		MMPF_IDXPSR_GetLittleLong(pCtxt->ulCurrentOffset, 4, &tag);
		pCtxt->ulCurrentOffset += 4;
		MMPF_IDXPSR_GetLittleLong(pCtxt->ulCurrentOffset, 4, &size);
		pCtxt->ulCurrentOffset += 4;
		
		if(tag == 0 && size == 0) {
		    return 0;
		}
		switch(tag){
		case ATOM_IDX1://MKTAG('i', 'd', 'x', '1'):
			nb_entries = size / 16;
            m_ulIDXTableEnd = pCtxt->ulCurrentOffset + size;
#if (SUPPORT_NO_IFLAG_SEEK == 1)    
            gulIframeSize[m_ubVideoCurTrack] = 0;
#endif 
			for(i = 0; i < nb_entries; i++)
			{
			    #if (VIDEO_BGPARSER_EN == 1)
			    if(m_bCloseBGParser)
	                break;
	            #endif
				index_pos = pCtxt->ulCurrentOffset;
				if(pCtxt->ulCurrentOffset > pCtxt->ulFileSize) {
				    goto the_end;
				}
				MMPF_IDXPSR_GetLittleLong(pCtxt->ulCurrentOffset, 4, &tag1);
				pCtxt->ulCurrentOffset += 4;
				MMPF_IDXPSR_GetLittleLong(pCtxt->ulCurrentOffset, 4, &flags);
				pCtxt->ulCurrentOffset += 4;
				MMPF_IDXPSR_GetLittleLong(pCtxt->ulCurrentOffset, 4, &pos);
				pCtxt->ulCurrentOffset += 4;
				MMPF_IDXPSR_GetLittleLong(pCtxt->ulCurrentOffset, 4, &len);
				pCtxt->ulCurrentOffset += 4;
				track_id = ((tag1 & 0xff) - '0') * 10;
				track_id += ((tag1 >> 8) & 0xff) - '0';
				if(track_id == pVidCtxTrack->ulTrack && (((tag1 >> 16) & 0xff) == 'd') && ((((tag1 >> 24) & 0xff) == 'c') || (((tag1 >> 24) & 0xff) == 'b'))){
				    *ulVideoEnd = pos + len + 8;
				    ulTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)pVidCtxTrack->ulFrameNum * (MMP_ULONG64)pVidCtxTrack->ulScale * (MMP_ULONG64)1000) / pVidCtxTrack->ulRate);
				    if(pVidCtxTrack->ulSampleSize == 0){
				        ulCurrentFrameNum = 1;
        	            pVidCtxTrack->ulFrameNum++;
        	        } else {
        	            ulCurrentFrameNum = (len / pVidCtxTrack->ulSampleSize);
        	            pVidCtxTrack->ulFrameNum += ulCurrentFrameNum;
        	        }
        	        //ulTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)pCtxt->VideoTrack.ulFrameNum * (MMP_ULONG64)pCtxt->VideoTrack.ulScale * (MMP_ULONG64)1000) / pCtxt->VideoTrack.ulRate);
#if (SUPPORT_NO_IFLAG_SEEK == 1)    
                    MMPF_AVIPSR_SupportNoIfraflagSeek(i, m_ubVideoCurTrack, len, &flags);
                    pVidSample->ulSize = len;
                    pVidSample->ulOffset = pos + gAVIVideoMedia[m_ubVideoCurTrack].ulMoviOffset + 8;                    
                    MMPF_AVIPSR_ChkFrameIfHasIflag(pVidSample, &flags);
#endif             
        	        if(flags & AVIIF_KEYFRAME){
        	            ulLastKeyOffset = pCtxt->ulCurrentOffset - 16;
                        ulLastKeyFrameNum = pVidCtxTrack->ulFrameNum - ulCurrentFrameNum;
                        pCtxt->ulKeyFrameNum++;
        	        }
        	        while(ulTime >= pVidCtxTrack->ulStoreTime && ulLastKeyOffset != 0) {
        	           *((MMP_ULONG *)(pVidCtxTrack->ulTableBuffer + pVidCtxTrack->ulTableWptr)) = ulLastKeyOffset;
        	           pVidCtxTrack->ulTableWptr += 4;
        	           *((MMP_ULONG *)(pVidCtxTrack->ulTableBuffer + pVidCtxTrack->ulTableWptr)) = ulLastKeyFrameNum;
        	           pVidCtxTrack->ulTableWptr += 4;
        	           pVidCtxTrack->ulStoreTime += pCtxt->ulDuration;
        	        }
        	        pVidCtxTrack->ulLastIdxOffset = pCtxt->ulCurrentOffset - 16;
				}
				if(track_id == pCtxt->AudioTrack.ulTrack && (((tag1 >> 16) & 0xff) == 'w') && (((tag1 >> 24) & 0xff) == 'b')){
				    *ulAudioEnd = pos + len + 8;
				    ulTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)pCtxt->AudioTrack.ulFrameNum * (MMP_ULONG64)pCtxt->AudioTrack.ulScale * (MMP_ULONG64)1000) / pCtxt->AudioTrack.ulRate);
				    if(pCtxt->AudioTrack.ulSampleSize == 0){
				        ulCurrentFrameNum = 1;
        	            pCtxt->AudioTrack.ulFrameNum++;
        	        } else {
        	            ulCurrentFrameNum = (len / pCtxt->AudioTrack.ulSampleSize);
        	            pCtxt->AudioTrack.ulFrameNum += ulCurrentFrameNum;
        	        }
        	        //ulTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)pCtxt->AudioTrack.ulFrameNum * (MMP_ULONG64)pCtxt->AudioTrack.ulScale * (MMP_ULONG64)1000) / pCtxt->AudioTrack.ulRate);
        	        while(ulTime >= pCtxt->AudioTrack.ulStoreTime) {
        	           *((MMP_ULONG *)(pCtxt->AudioTrack.ulTableBuffer + pCtxt->AudioTrack.ulTableWptr)) = pCtxt->ulCurrentOffset - 16;
        	           pCtxt->AudioTrack.ulTableWptr += 4;
        	           *((MMP_ULONG *)(pCtxt->AudioTrack.ulTableBuffer + pCtxt->AudioTrack.ulTableWptr)) = pCtxt->AudioTrack.ulFrameNum - ulCurrentFrameNum;
        	           pCtxt->AudioTrack.ulTableWptr += 4;
        	           pCtxt->AudioTrack.ulStoreTime += pCtxt->ulDuration;
        	        }
        	        pCtxt->AudioTrack.ulLastIdxOffset = pCtxt->ulCurrentOffset - 16;
				}
			}
			break;
		default:
		//skip:
			size += (size & 1);
			pCtxt->ulCurrentOffset += size;
			break;
		}
	}
the_end:
	return err;
}


MMP_ERR MMPF_IDXPSR_GetNextSuperIdx(MMP_AVI_MEDIA_INFO *pMedia)
{
    MMP_ERR err = MMP_ERR_NONE;
    MMP_ULONG ulHighL, ulLowL;
    
    if(pMedia->ulSpIdxCount >= pMedia->ulSpIdxNum){
        return MMP_VIDPSR_ERR_EOS;
    }
    
    if((pMedia->ulSpIdxOffset + 16) > pMedia->ulFileSize)
        return MMP_VIDPSR_ERR_EOS;
    
	MMPF_IDXPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &ulLowL);
	pMedia->ulSpIdxOffset += 4;
	MMPF_IDXPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &ulHighL);
	pMedia->ulSpIdxOffset += 4;
	pMedia->SpIdxEntry.ullOffset = ((MMP_ULONG64)ulHighL << 32) | ulLowL;
	MMPF_IDXPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &pMedia->SpIdxEntry.ulSize);
	pMedia->ulSpIdxOffset += 4;
	MMPF_IDXPSR_GetLittleLong(pMedia->ulSpIdxOffset, 0, &pMedia->SpIdxEntry.ulDuration);
	pMedia->ulSpIdxOffset += 4;
	
	pMedia->ulSpIdxCount++;

	pMedia->ulIdxOffset = (MMP_ULONG)pMedia->SpIdxEntry.ullOffset;
	if((pMedia->ulIdxOffset + 32) > pMedia->ulFileSize)
	    return MMP_VIDPSR_ERR_EOS;
	pMedia->ulIdxOffset += 12;
	MMPF_IDXPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &pMedia->ulStdIdxNum);
	pMedia->ulIdxOffset += 4;
	pMedia->ulStdIdxCount = 0;
	pMedia->ulIdxOffset += 4; // skip ChunkID(4)
	
	MMPF_IDXPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &ulLowL);
	pMedia->ulIdxOffset += 4;
	MMPF_IDXPSR_GetLittleLong(pMedia->ulIdxOffset, 0, &ulHighL);
	pMedia->ulIdxOffset += 4;
	pMedia->ullBaseOffset = ((MMP_ULONG64)ulHighL << 32) | ulLowL;
	pMedia->ulIdxOffset += 4; //Skip reserved(4)
	
	pMedia->ulReadCount = 32;
	return err;
}

MMP_ERR MMPF_IDXPSR_ParseSuperIdx(void *pContext, MMP_AVI_MEDIA_INFO *pMedia)
{
    MMP_ERR err = MMP_ERR_NONE;
    MMP_ULONG size;
	MMP_ULONG nb_entries = 0;
	MMP_ULONG len;
	MMP_ULONG ulTime = 0;
	MMP_ULONG ulLastKeyOffset = 0;
	MMP_ULONG ulLastKeyFrameNum = 0;
	MMP_ULONG ulCurrentFrameNum = 0;
	MMP_IDX_AVIMEDIA_INFO *pAviCtx = NULL;
	MMP_IDX_AVITRACK_INFO *pTrack = NULL;
    
	pAviCtx = (MMP_IDX_AVIMEDIA_INFO *)pContext;
	
	if(pMedia->ulCodecType == CODEC_TYPE_VIDEO){
	    pTrack = &pAviCtx->VideoTrack[m_ubVideoCurTrack];
	} else if(pMedia->ulCodecType == CODEC_TYPE_AUDIO){
	    pTrack = &pAviCtx->AudioTrack;
	} else {
	    return MMP_ERR_NONE;
	}
	
    while(1) {
        #if (VIDEO_BGPARSER_EN == 1)
	    if(m_bCloseBGParser)
            break;
        #endif
        if((pMedia->ulReadCount >= pMedia->SpIdxEntry.ulSize) || (pMedia->ulStdIdxCount >= pMedia->ulStdIdxNum)){ // Read Next Super index table entry
            err = MMPF_IDXPSR_GetNextSuperIdx(pMedia);
        }
        if(err != MMP_ERR_NONE)
            return err;
        
        if((pMedia->ulIdxOffset + 8) > pMedia->ulFileSize)
            return MMP_VIDPSR_ERR_EOS;
        if(pTrack->ulTableWptr >= AVI_INDEX_BUFFER_SIZE)
            return MMP_VIDPSR_ERR_EOS;
        
	    MMPF_IDXPSR_GetLittleLong(pMedia->ulIdxOffset + 4, 0, &len);
	    
		size = len & AVI_SAMPLESIZEMASK;
		
		if(size >= pMedia->ulFileSize) {
		    return MMP_VIDPSR_ERR_EOS;
		}

        ulTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)pTrack->ulFrameNum * (MMP_ULONG64)pTrack->ulScale * (MMP_ULONG64)1000) / pTrack->ulRate);
	    if(pTrack->ulSampleSize == 0){
	        ulCurrentFrameNum = 1;
	        pTrack->ulFrameNum++;
	    } else {
	        ulCurrentFrameNum = (size / pTrack->ulSampleSize);
        	pTrack->ulFrameNum += ulCurrentFrameNum;
	    }
	    
		if(len & AVI_ISKEYFRAME) {
		} else {
		    ulLastKeyOffset = nb_entries;
            ulLastKeyFrameNum = pTrack->ulFrameNum - ulCurrentFrameNum;
		}
		if(pMedia->ulCodecType == CODEC_TYPE_AUDIO){
		    ulLastKeyOffset = nb_entries;
            ulLastKeyFrameNum = pTrack->ulFrameNum - ulCurrentFrameNum;
		}
        while(ulTime >= pTrack->ulStoreTime) {
            if(pTrack->ulTableWptr >= AVI_INDEX_BUFFER_SIZE)
                return MMP_VIDPSR_ERR_EOS;
            *((MMP_ULONG *)(pTrack->ulTableBuffer + pTrack->ulTableWptr)) = ulLastKeyOffset;
            pTrack->ulTableWptr += 4;
            *((MMP_ULONG *)(pTrack->ulTableBuffer + pTrack->ulTableWptr)) = ulLastKeyFrameNum;
            pTrack->ulTableWptr += 4;
            pTrack->ulStoreTime += pAviCtx->ulDuration;
        }
        pMedia->ulIdxOffset += 8;
        pMedia->ulReadCount += 8;
        pMedia->ulStdIdxCount++;
	    nb_entries++;
    }
    return MMP_ERR_NONE;
}

MMP_BOOL MMPF_IDXPSR_GetSuperIdxFlag(void)
{
    return m_bUseAudioSuperIdx;
}

MMP_ERR MMPF_IDXPSR_SeekKeySample(MMP_SHORT sAdvance, MMP_BOOL *bNoIndex, MMP_ULONG *ulIdxOffset, MMP_ULONG *ulSampleNum)
{
    MMP_IDX_AVIMEDIA_INFO *pAviCtx = &m_IndexContext;
    MMP_ULONG ulFrameNum = *ulSampleNum;
    MMP_ULONG ulTime = 0, ulPtr = 0;
    MMP_ULONG ulTempNum = 0;
    
    if(pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableWptr == 0) {
        *bNoIndex = MMP_TRUE;
        return MMP_ERR_NONE;
    }
    if(sAdvance > 0) {
        ulFrameNum++;
    } else {
        if(ulFrameNum > 0)
            ulFrameNum--;
    }
    ulTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)ulFrameNum * (MMP_ULONG64)pAviCtx->VideoTrack[m_ubVideoCurTrack].ulScale * (MMP_ULONG64)1000) / pAviCtx->VideoTrack[m_ubVideoCurTrack].ulRate);
    ulPtr = (ulTime / pAviCtx->ulDuration) << 3;
    while(sAdvance != 0) {
        if(sAdvance > 0) {
            if(ulPtr > (pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableWptr - 8)) {
                ulPtr = pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableWptr - 8;
            }
            ulTempNum = *((MMP_ULONG *)(pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableBuffer + ulPtr + 4));
            if(ulTempNum > ulFrameNum) {
                sAdvance--;
                ulFrameNum = ulTempNum;
            }
            ulPtr += 8;
            if(sAdvance == 0 || ulPtr >= pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableWptr) {
                *ulIdxOffset = *((MMP_ULONG *)(pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableBuffer + ulPtr));
                *ulSampleNum = ulTempNum;
                if(sAdvance == 0)
                    return MMP_ERR_NONE;
                else
                    return MMP_VIDPSR_ERR_EOS;
            }
        } else if (sAdvance < 0) {
            ulTempNum = *((MMP_ULONG *)(pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableBuffer + ulPtr + 4));
            if(ulTempNum < ulFrameNum) {
                sAdvance++;
                ulFrameNum = ulTempNum;
            }
            if(sAdvance == 0 || ulPtr == 0) {
                *ulIdxOffset = *((MMP_ULONG *)(pAviCtx->VideoTrack[m_ubVideoDmxTrackNo].ulTableBuffer + ulPtr));
                *ulSampleNum = ulTempNum;
                if(sAdvance == 0)
                    return MMP_ERR_NONE;
                else
                    return MMP_VIDPSR_ERR_EOS;
            }
            ulPtr -= 8;
        }
    }
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_IDXPSR_SeekSample(MMP_ULONG ulTime, MMP_ULONG *ulPtr, MMP_ULONG *ulIdxOffset, 
                        MMP_ULONG *ulSampleNum, MMP_ULONG *ulLastOffset, MMP_ULONG ulFrame, MMP_BOOL bVideo, MMP_BOOL bBackward)
{
    MMP_IDX_AVITRACK_INFO *pAviCtxTrack;
    *ulPtr = ((ulTime + m_IndexContext.ulDuration - 1) / m_IndexContext.ulDuration) << 3; // 8 bytes per entry
    if(bVideo) {
        pAviCtxTrack = &m_IndexContext.VideoTrack[m_ubVideoCurTrack];
    } else {
        pAviCtxTrack = &m_IndexContext.AudioTrack;
    }
    if(pAviCtxTrack->ulTableWptr == 0) {
        *ulPtr = 0xFFFFFFFF;
        return MMP_ERR_NONE;
    }
    *ulLastOffset = pAviCtxTrack->ulLastIdxOffset;
    if(*ulPtr > (pAviCtxTrack->ulTableWptr - 8)) {
        *ulPtr = pAviCtxTrack->ulTableWptr - 8;
    }
    *ulIdxOffset = *((MMP_ULONG *)(pAviCtxTrack->ulTableBuffer + *ulPtr));
    *ulSampleNum = *((MMP_ULONG *)(pAviCtxTrack->ulTableBuffer + *ulPtr + 4));  
    if(bBackward) {
    	while(*ulSampleNum > ulFrame) {
    	    if(*ulPtr == 0)
            	break;
    		*ulPtr -= 8;
    		*ulIdxOffset = *((MMP_ULONG *)(pAviCtxTrack->ulTableBuffer + *ulPtr));
            *ulSampleNum = *((MMP_ULONG *)(pAviCtxTrack->ulTableBuffer + *ulPtr + 4));
    	}
    }  
    if(!bBackward) {
        while(*ulSampleNum <= ulFrame) {
            *ulPtr += 8;
            if(*ulPtr >= pAviCtxTrack->ulTableWptr) {
                *ulPtr -= 8;
                break;
            }
            *ulIdxOffset = *((MMP_ULONG *)(pAviCtxTrack->ulTableBuffer + *ulPtr));
            *ulSampleNum = *((MMP_ULONG *)(pAviCtxTrack->ulTableBuffer + *ulPtr + 4));
        }
    }
    return MMP_ERR_NONE;
}

/** @} */ // end of AVI Demuxer

#if 0
void _____AVIParser_APIs_____(){}
#endif

MMP_ERR MMPF_AVIPSR_GetFileTime(MMP_ULONG *total_time, MMP_ULONG *video_time, MMP_ULONG *audio_time)
{
    MMP_BOOL bGetAudioTime = MMP_FALSE;
    MMP_IDX_AVIMEDIA_INFO *pAviCtx = &m_IndexContext;
    MMP_AVI_MEDIA_INFO *pVidMedia = &gAVIVideoMedia[m_ubVideoCurTrack];
    if(pAviCtx->VideoTrack[m_ubVideoCurTrack].ulTableWptr != 0) {
        *video_time = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)((MMP_ULONG64)pAviCtx->VideoTrack[m_ubVideoCurTrack].ulFrameNum * (MMP_ULONG64)pVidMedia->ulScale) * 1000) / pVidMedia->ulRate);
        *total_time = *video_time;
    }
    if(pAviCtx->AudioTrack.ulTableWptr != 0) {
        *audio_time = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)((MMP_ULONG64)pAviCtx->AudioTrack.ulFrameNum * (MMP_ULONG64)gAVIAudioMedia.ulScale) * 1000) / gAVIAudioMedia.ulRate);
        bGetAudioTime = MMP_TRUE;
    }
    if(bGetAudioTime) {
        if(*audio_time > *video_time)
            *total_time = *audio_time;
    }
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_AVIPSR_CheckVideoSeekable(MMP_ULONG ulTime, MMP_BOOL bCheckVideoOnly)
{
    MMP_ULONG RequestFrame = 0;
    MMP_BOOL  bVideoEOS = 0, bAudioEOS = 0;
    MMP_IDX_AVIMEDIA_INFO *pAviCtx = &m_IndexContext;
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoCurTrack];
    if(!(pVidMedia->bIsSuperIdx)) {
        if(pAviCtx->VideoTrack[m_ubVideoCurTrack].ulTableWptr == 0) {
            return MMP_VIDPSE_ERR_CONTENT_CANNOTSEEK;
        }
        RequestFrame = MMPF_AVIPSR_GetSampleNumByTime(ulTime, pVidMedia);
        if(RequestFrame > pAviCtx->VideoTrack[m_ubVideoCurTrack].ulFrameNum) {
            bVideoEOS = MMP_TRUE;
        }
    }
    if(!(gAVIAudioMedia.bIsSuperIdx)) {
    	if(pAviCtx->AudioTrack.ulTableWptr == 0) {
            bAudioEOS = MMP_TRUE;
        }
        RequestFrame = MMPF_AVIPSR_GetSampleNumByTime(ulTime, &gAVIAudioMedia);
        if(RequestFrame > pAviCtx->AudioTrack.ulFrameNum) {
            bAudioEOS = MMP_TRUE;
        }
    }
    if(bCheckVideoOnly) {
        bAudioEOS = MMP_TRUE;
    }
    if(bAudioEOS && bVideoEOS) {
    	return MMP_VIDPSE_ERR_CONTENT_CANNOTSEEK;
    }
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_AVIPSR_BuildIndexTable(void)
{
    MMP_ULONG ulVideoEnd = 0xFFFFFFFF, ulAudioEnd = 0xFFFFFFFF;
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoCurTrack];    
    MMP_IDX_AVIMEDIA_INFO *pAviCtx = &m_IndexContext;
    m_bBuildTableDone = MMP_FALSE;
    m_bUseAudioSuperIdx = MMP_FALSE;
    
    #if (VIDEO_BGPARSER_EN == 1)
    m_bCloseBGParser = MMP_FALSE;
    MMPF_OS_ReleaseSem(gBGParser_SemID);
    #else
    if(!gIDXVideoMedia.bIsSuperIdx) {

        if (m_MFDFormat == MMP_VID_VIDEO_MJPG)
        {
            MMPF_IDXPSR_ParseIndexTable(&m_IndexContext, gIDXVideoMedia.ulIdx1start, &ulVideoEnd, &ulAudioEnd);
        }
        else
        {
            MMPF_IDXPSR_ParseIndexTable(&m_IndexContext, gIDXVideoMedia.ulMoviEnd, &ulVideoEnd, &ulAudioEnd);
        }
        if(pAviCtx->ulKeyFrameNum <= 1) {
            gVidFileInfo.is_video_seekable[m_ubVideoCurTrack] = MMP_FALSE;
        }

        if((pVidMedia->ulIndexMoviOffset + ulVideoEnd) > pVidMedia->ulIndexMoviOffset) {
            pVidMedia->ulDataEnd = pVidMedia->ulIndexMoviOffset + ulVideoEnd;
        }
        if((gAVIAudioMedia.ulIndexMoviOffset + ulAudioEnd) > gAVIAudioMedia.ulIndexMoviOffset) {
            gAVIAudioMedia.ulDataEnd = gAVIAudioMedia.ulIndexMoviOffset + ulAudioEnd;
        }
    }

    if(pAviCtx->AudioTrack.ulTableWptr == 0 && gIDXAudioMedia.bIsSuperIdx) {
        MMPF_IDXPSR_ParseSuperIdx(&m_IndexContext, &gIDXAudioMedia);
        m_bUseAudioSuperIdx = MMP_TRUE;
    }
    m_bBuildTableDone = MMP_TRUE;
    #endif
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_AVIPSR_InitIDXPSRContext(MMP_ULONG ulFSize, MMP_ULONG ulVideoBuf, MMP_ULONG ulAudioBuf, MMP_ULONG ulBufSize, MMP_ULONG ulTotalTime)
{ 
	MMP_ERR	err = MMP_ERR_NONE;
	MMP_IDX_AVIMEDIA_INFO *pContext = MMPF_IDXPSR_GetConText();
    MMP_AVI_MEDIA_INFO  *pVidMedia = &gAVIVideoMedia[m_ubVideoCurTrack];        	
	
	MEMSET0(pContext);
	// file related
    pContext->ulFileSize = ulFSize;
    pContext->VideoTrack[m_ubVideoCurTrack].ulTrack = pVidMedia->ulTrackID;
    pContext->AudioTrack.ulTrack = gAVIAudioMedia.ulTrackID;
    pContext->VideoTrack[m_ubVideoCurTrack].ulSampleSize = pVidMedia->ulSampleSize;
    pContext->AudioTrack.ulSampleSize = gAVIAudioMedia.ulSampleSize;
    pContext->VideoTrack[m_ubVideoCurTrack].ulScale = pVidMedia->ulScale;
    pContext->AudioTrack.ulScale = gAVIAudioMedia.ulScale;
    pContext->VideoTrack[m_ubVideoCurTrack].ulRate = pVidMedia->ulRate;
    pContext->AudioTrack.ulRate = gAVIAudioMedia.ulRate;
    // index memory table related
    pContext->VideoTrack[m_ubVideoCurTrack].ulTableBuffer = ulVideoBuf;
    pContext->AudioTrack.ulTableBuffer = ulAudioBuf;
    pContext->VideoTrack[m_ubVideoCurTrack].ulLastIdxOffset = ulFSize;
    pContext->AudioTrack.ulLastIdxOffset = ulFSize;
    pContext->ulMaxEntry = ulBufSize >> 3;
    pContext->ulDuration = ulTotalTime / pContext->ulMaxEntry;
    pContext->ulKeyFrameNum = 0;
    if(pContext->ulDuration < AVI_MIN_INDEX_DURATION)
        pContext->ulDuration = AVI_MIN_INDEX_DURATION;
	return err;
}

/** @brief reset the parser variables

Reset the variable so that the file could be parsed again.
*/
MMP_ERR MMPF_AVIPSR_Init(void)
{
    MMP_ULONG ulTemp;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];    
    // Reset sample information
  
	MEMSET0(&gAVIAudioSample);
    MEMSET0(&gAVIVideoSample[0]);
    MEMSET0(&gAVIVideoSample[1]);

    // Reset seek information
    MEMSET0(&m_seek[0]);
    MEMSET0(&m_seek[1]);

    // Reset audio specific config
	MEMSET0(&mp4ASC);

    // Reset media information
    MEMSET0(&gAVIAudioMedia);
    MEMSET0(&gAVIVideoMedia[0]);
    MEMSET0(&gAVIVideoMedia[1]);

    // Reset 3GP/AVI private data
    MEMSET0(&m_3GPPdata);
    MEMSET0(&gAVIDataMedia);
    MEMSET0(&gAVITextMedia);

    // Reset 3GP user data
    //MEMSET0(&m_uDtatag);

    // Reset file information, Keep the file size.
    ulTemp = gVidFileInfo.file_size;
    
    MEMSET0(&gVidFileInfo);
    
    gVidFileInfo.file_size = ulTemp;
	
    // Default set to 1x speed
    pSeek->sSpeed = 10; 
    gDemuxOption  = 0;
	
	m_ulDemuxErrorCount	= 0;
	
	gAVIVideoMedia[m_ubVideoCurTrack].ulDataEnd    = 0xFFFFFFFF;
	gAVIAudioMedia.ulDataEnd    = 0xFFFFFFFF;
	
    #if (AIT_DIVXDRM_SUPPORT == 1)
    m_ulPreAudioChunkSize = 0;
    #endif
	
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_AVIPSR_Deinit(void)
{
    return MMP_ERR_NONE;
}

/** @brief Demux Audio/Video frame from the bitstream

Triggered by timer2.
Fullfill Audio buffer and Get one Video frame from the bitstream
@pre The caller should avoid playing video which is neither h.263 nor MP4VSP
@return The status of the function call
*/
MMP_ERR MMPF_AVIPSR_Demux(void)
{
    MMP_ERR                 errEOS = MMP_ERR_NONE;// If it's end of strem this time
    MMP_ERR                 error = MMP_ERR_NONE;
    MMP_ULONG               ulBackupRequestFrame,ulRequestFrame = 0;
    MMP_ULONG               ulSizeToLoad;
    MMP_ULONG 	            ulHeaderAddr, ulFrameAddr, ulAvailSize, ulAvailSize1, ulDataSize;
	MMPF_3GPP_SEEKSTATUS    *pSeek = &m_seek[m_ubVideoDmxTrackNo];
    
    // Demux audio data
	if (gDemuxOption & DEMUX_AUDIO) {
        
	    // clear the demux audio option
		gDemuxOption &= ~DEMUX_AUDIO;
		
        // fill audio data
        {
            MMP_USHORT  usXferSize = 0,usXferSize1 = 0;
            MMP_ULONG   ulAddr,ulAddr1;
            MMP_LONG    ulAvailSize,ulAvailSize1;
            MMP_ULONG   ulReturnSize;
                
            ulReturnSize = 0;
                
            //if (gAudioSample.ulOffset != 0) 
            {

                if (MMPF_BS_GetAudioStartAddr(&ulAddr,&ulAddr1,&ulAvailSize,&ulAvailSize1) == MMP_ERR_NONE) {
                    if (ulAvailSize1 < 0) { //doesn't need to rewind, plain read
                        error = MMPF_AVIPSR_FillAudioBuffer(ulAddr,ulAvailSize, &usXferSize);
                        MMPF_BS_UpdateAudioBufptr(usXferSize,MMP_FALSE);
                        if(error != MMP_VIDPSR_ERR_EOS && error != MMP_ERR_NONE)
                            return error;
                    }
                    else {// not enough size to put all data, have to rewind
                        error = MMPF_AVIPSR_FillAudioBuffer(ulAddr,ulAvailSize, &usXferSize);

                        if(usXferSize >= ulAvailSize) {
                            MMPF_BS_UpdateAudioBufptr(usXferSize,MMP_TRUE);

                            error = MMPF_AVIPSR_FillAudioBuffer(ulAddr1,ulAvailSize1, &usXferSize1);
                            MMPF_BS_UpdateAudioBufptr(usXferSize1,MMP_FALSE);
                            usXferSize += usXferSize1;
                        } else {
                            MMPF_BS_UpdateAudioBufptr(usXferSize,MMP_FALSE);
                        }
                        if(error != MMP_VIDPSR_ERR_EOS && error != MMP_ERR_NONE)
                            return error;
                    }
                }
            }
        }
	}

	// Demux video data
    if ( gDemuxOption & DEMUX_VIDEO ) {
    
        // Clear the demux video option
        gDemuxOption &= ~(DEMUX_VIDEO);

 	    // Fill video data        
        if (!m_3GPPdata.bFillBufferDone) {

            if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame))
            {   
                ulBackupRequestFrame = pSeek->ulRequestFrame;
                
                //WRITE_HEADER->ulRequestTime = MMPF_AVIPSR_GetTime(&gAVIVideoMedia[m_ubVideoDmxTrackNo]);
                
                errEOS = MMPF_AVIPSR_SeekNextVideoFrame(&ulRequestFrame);

                if ((errEOS == MMP_VIDPSR_ERR_EOS) && (m_bVideoEOS)) {
                    return MMP_VIDPSR_ERR_EOS;
                }
                
                if(errEOS == MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA) {
                    return MMP_ERR_NONE;
                }
                
                pSeek->ulRequestFrame = ulBackupRequestFrame;
                // ===== Dual Track =====
		        WRITE_HEADER->ubTrackNo     = m_ubVideoDmxTrackNo;
		        WRITE_HEADER->ubIsDualTrack = gubIsVidDualTrks;
                // ===== Time stamp =====                
                WRITE_HEADER->ulRequestTime = MMPF_AVIPSR_GetTime(&gAVIVideoMedia[m_ubVideoDmxTrackNo]);                

                if((pSeek->mode == MMPF_M_FF_KEY || pSeek->mode == MMPF_M_SEEK_NONE)) {
                    if(pSeek->sSpeed > 0 && gAVIVideoMedia[m_ubVideoDmxTrackNo].ulFrameIndex > 0) {
                        gAVIVideoMedia[m_ubVideoDmxTrackNo].ulFrameIndex--;
                        WRITE_HEADER->ulRequestTime = MMPF_AVIPSR_GetTime(&gAVIVideoMedia[m_ubVideoDmxTrackNo]);
                        gAVIVideoMedia[m_ubVideoDmxTrackNo].ulFrameIndex++;
                    } else {
                        WRITE_HEADER->ulRequestTime = MMPF_AVIPSR_GetTime(&gAVIVideoMedia[m_ubVideoDmxTrackNo]);
                    }
                }
                if ((MMP_ERR_NONE == errEOS) || (MMP_VIDPSR_ERR_EOS == errEOS)) {
                    m_3GPPdata.bFilling = MMP_TRUE;
                }
                else {
                    DBG_S(3, " Seek Error\r\n");
                    return errEOS;
                }
            }
            
            if ( errEOS == MMP_VIDPSR_ERR_EOS) {
                m_bVideoEOS = MMP_TRUE;
            }
            
            ulDataSize = gAVIVideoSample[m_ubVideoDmxTrackNo].ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding;

            error = MMPF_BS_GetVideoStartAddr(&ulHeaderAddr,
                                              &ulFrameAddr,
                                              &ulAvailSize,
                                              &ulAvailSize1,
                                              m_3GPPdata.usBSOffset,
                                              &ulDataSize,
                                              m_ulBsAlignType);
            //if (m_ulBsAlignType == BS_ALIGN_256) {                        
            //    printc(AT_DEFAULT("SRC_DATA:%x SIZE:%x(STSZ:%x)\r\n"), ulFrameAddr, ulDataSize, gAVIVideoSample[m_ubVideoDmxTrackNo].ulSize);            
            //}        
            if (error == MMP_ERR_NONE) {
                m_3GPPdata.bLargeFrame = MMP_FALSE;
            }
            else if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
                if (m_bBsSupportLargeFrame) {
                    m_3GPPdata.bLargeFrame  = MMP_TRUE;
                }
                else {
                    m_3GPPdata.bLargeFrame = MMP_FALSE;
                    m_3GPPdata.bFilling    = MMP_FALSE; //skip this frame
                    return error;
                }
            }
            else {
                return error;
            }
            
            pSeek->ulRequestFrame       = ulRequestFrame;
            
            // ===== Update the final frame size =====
            if (m_3GPPdata.bLargeFrame) {
                WRITE_HEADER->ulSize    = ulAvailSize + ulAvailSize1;
            }
            else {
                WRITE_HEADER->ulSize    = ulDataSize;
            }
            
            ulAvailSize -= m_3GPPdata.usBSOffset;
            
            WRITE_HEADER->ulBSSize      = gAVIVideoSample[m_ubVideoDmxTrackNo].ulSize;
        	WRITE_HEADER->ulAddress     = ulFrameAddr;
        	WRITE_HEADER->ulFlag        = 0;
            WRITE_HEADER->ulFrameNum    = gAVIVideoMedia[m_ubVideoDmxTrackNo].ulFrameIndex;

            if (gAVIVideoSample[m_ubVideoDmxTrackNo].ulSize < (ulAvailSize+ulAvailSize1)) {
                
                ulSizeToLoad = gAVIVideoSample[m_ubVideoDmxTrackNo].ulSize;
                
                WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
                
                if(m_bVideoEOS) {
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
        	    }
            }
            else {
                ulSizeToLoad = (ulAvailSize+ulAvailSize1);
            }
            
            gAVIVideoSample[m_ubVideoDmxTrackNo].ulSize -= ulSizeToLoad;
            
            // copy the write header into the bitstream
            MMPF_BS_AccessVideoHeader((MMP_UBYTE*)ulHeaderAddr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_FRAME_INFO),RING_WRITE);

            // Put single VOP for video decoder          
            error = MMPF_AVIPSR_GetSingleFrame(WRITE_HEADER->ulAddress,ulSizeToLoad,ulAvailSize,ulAvailSize1,&(gAVIVideoSample[m_ubVideoDmxTrackNo].ulOffset));            
#if (DEBUG_VIDEO & DUMP_BS_BUFDATA)
            if (m_ubVideoDmxTrackNo) 
            {
                MMPF_AVIPSR_DumpCompBuf(WRITE_HEADER->ulAddress, ulSizeToLoad);
            }                
#endif             
            #if (AIT_DIVXDRM_SUPPORT == 1)
            if (DRM_DIVX_IsEnable()) {
                // discript, video data adress : WRITE_HEADER->ulAddress, size : SizeToLoad, key : m_VideoKey
                DRM_DIVX_DecryptVideo((MMP_UBYTE *)WRITE_HEADER->ulAddress, ulSizeToLoad);
            }
            #endif

            if (error != MMP_ERR_NONE) {
                m_3GPPdata.bFilling         = MMP_FALSE;
                m_3GPPdata.bFillBufferDone  = MMP_FALSE;
            
                return error;
            }
            
            m_3GPPdata.bFillBufferDone = MMP_TRUE;
            
#if (DEBUG_VIDEO & LOG_VID_BUF)     
            if (m_ubVideoCurTrack == m_ubVideoDmxTrackNo)
            {
                printc(FG_GRAY("[AVI]VID_DMX=%x\r\n"), WRITE_HEADER->ulFrameNum + (m_ubVideoDmxTrackNo<<28));       
            }      
#endif
            MMPF_BS_UpdateVideoBufPtr(RING_WRITE, WRITE_HEADER->ulSize);
            
            m_3GPPdata.bFilling         = MMP_FALSE;
            m_3GPPdata.bFillBufferDone  = MMP_FALSE; 
            m_3GPPdata.pPreWriteHeader.ulRequestTime  = ((MMPF_M_FRAME_INFO*)ulHeaderAddr)->ulRequestTime;
#if 0
            if (gubIsVidDualTrks) {
                //if (!m_ulDisVidDualDmux)
                {
                    m_ubVideoDmxTrackNo++;                
                    m_ubVideoDmxTrackNo &= 0x01;
                }
            }
#endif            
        } // ! fill buffer done
    }

    return  MMP_ERR_NONE;
}

MMP_VID_FILEVALIDATION MMPF_AVIPSR_ParseEx(MMP_VIDEO_CONTAINER_INFO* pInfo, MMP_BOOL bInitCodec)
{
	AVIContext  AviCtx;
	MMP_ULONG   ulTag, ulTag1, ulSize, tmp;
	MMP_ULONG   handler;
	MMP_AVI_MEDIA_INFO *pMedia;
	MMP_LONG	stream_index = -1, codec_type;
	MMP_LONG    lFilePos;
	MMP_UBYTE   *dest_buf       = m_VideoStreamBuf;
	MMP_ULONG   ulMetaSize      = 0;
	//MMP_BOOL    bSkipHeader     = MMP_FALSE;
	MMP_ERR     error           = MMP_ERR_NONE;
	MMP_BOOL    bVideoInitFail  = MMP_FALSE;
	MMP_ULONG   ulWorkingBuf    = m_ulWorkingBufStart;
	#if (SPEED_UP_AVI_SEEK)
	MMP_ULONG   ulIDXVBuffer    = 0;
	MMP_ULONG   ulIDXABuffer    = 0;
	#endif
	MMP_BOOL    bUndefinedTime  = MMP_FALSE;
    MMP_UBYTE   ubCurVidIndex 	= 0;    
    MMP_UBYTE   ubVideoCurTrackTmp = 0;    
    MMP_AVI_MEDIA_INFO *pVidMedia = &gAVIVideoMedia[m_ubVideoCurTrack];
    MMP_AVI_SAMPLE_INFO *pVidSample = &gAVIVideoSample[m_ubVideoCurTrack];
    MMP_VIDEO_INFO *pVidInfo = &gVidFileInfo.video_info[m_ubVideoCurTrack];
    #ifdef BUILD_FW
	#if (SPEED_UP_AVI_SEEK)
    // Will Add, Init working buffer
    ulWorkingBuf = ALIGN4K(ulWorkingBuf);
    ulIDXVBuffer = ulWorkingBuf;
    ulWorkingBuf += AVI_INDEX_BUFFER_SIZE;
    ulIDXABuffer = ulWorkingBuf;
    ulWorkingBuf += AVI_INDEX_BUFFER_SIZE;
    m_ulFastSeekBufStart = ulWorkingBuf;
    ulWorkingBuf += FASTSEEK_BUFFER_SIZE;
    #if (AIT_DIVXDRM_SUPPORT == 1)  
    m_ulAudioTempBuf = ulWorkingBuf;
    ulWorkingBuf += AUDIO_TEMP_BS_SIZE;
    #endif
    m_ulTempBSSize = 0;
    m_ulWorkingBufSize = ulWorkingBuf - m_ulWorkingBufStart;
    // end init
	#endif
    #endif

    m_ulFirstCTime 			= 0;
    m_ulFirstTime 			= 0;

    m_ulAudioPtr 			= 0; 
	m_ulPreAudioOffset      = 0;
	m_ulPreAudioReduntSize  = 0;
 
	m_ulCurFileOffset 		= 0;
	m_ulVideoTrackTotalCnt 	= 0; 

	(void)MMPF_AVIPSR_Init();

	MEMSET(&AviCtx,0, sizeof(AviCtx));
	
	//m_ulPreAudioOffset      = 0;
	//m_ulPreAudioReduntSize  = 0;
	
	m_ulAudioJumpTime = 0;
	
	MMPF_VIDPSR_ResetCache();
	MMPF_VIDPSR_SwapDepackCacheIdx(INIT_CACHE);
    
    m_ulCurFileOffset = 0;
    m_ulAudioPtr = 0;
    
	MMPF_AVIPSR_GetRIFFHeader(&AviCtx);
	
	pVidMedia->ulFileSize = gVidFileInfo.file_size;

    while (m_ulCurFileOffset < gVidFileInfo.file_size)
	{
		MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &ulTag);
		MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &ulSize);

        if (gbAllowListAtomEn) {
        	if (ulTag == glAllowListAtom) {
                ulTag = ATOM_LIST;
                //printc(FG_YELLOW("Allow LIST ATOM Modifying\r\n"));
            }
        }
        
		switch(ulTag){
			case ATOM_LIST:
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &ulTag1);

				if (ulTag1 == ATOM_MOVI) {
				    lFilePos = m_ulCurFileOffset;
					AviCtx.ulMoviList = lFilePos - 4;                    
					if(ulSize) {AviCtx.ulMoviEnd = AviCtx.ulMoviList + ulSize + (ulSize & 1);}
                    pMedia->ulIdx1start = AviCtx.ulMoviEnd;  
                    //printc(FG_YELLOW("movi:%x; idx1:%x\r\n"), AviCtx.ulMoviList, pMedia->ulIdx1start);                    
                    MMPF_AVIPSR_SkipFile(ulSize - 4);
					//goto End_of_Header;
				}
				break;
			case ATOM_DMLH://MKTAG('d', 'm', 'l', 'h'):
				MMPF_AVIPSR_SkipFile(ulSize + (ulSize & 1));
				break;
			case ATOM_AMVH://MKTAG('a', 'm', 'v', 'h'):
				goto fail;
			case ATOM_AVIH://MKTAG('a', 'v', 'i', 'h'):
				/* AVI header */
				/* using frame_period is bad idea */
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &avih.frame_period);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &avih.bit_rate);
				avih.bit_rate *= 8;
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);
				AviCtx.ulHaveIndex = ((tmp & AVIF_HASINDEX) != 0);

				MMPF_AVIPSR_SkipFile(4*4);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &avih.width);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &avih.height);
				MMPF_AVIPSR_SkipFile(ulSize - 10*4);
				break;
			case ATOM_STRH://MKTAG('s', 't', 'r', 'h'):
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &ulTag1);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &handler);
				if (ulTag1 == ATOM_PADS){
					MMPF_AVIPSR_SkipFile(ulSize - 8);
					break;
				}
				else {
					stream_index++;
				}
				
				switch(ulTag1) {
				case ATOM_VIDS://MKTAG('v', 'i', 'd', 's'):
                    if (m_ulVideoTrackTotalCnt) {
                        ubCurVidIndex++;
                    }
				    //if (gVidFileInfo.is_video_available[0]) {
				    //    bSkipHeader = MMP_TRUE;
				    //} 
				    //else {
    					pMedia = &gAVIVideoMedia[ubCurVidIndex];
    					gVidFileInfo.is_video_available[ubCurVidIndex] = 1;
    					pMedia->ulTrackID = stream_index;
    					pMedia->ulCodecType = CODEC_TYPE_VIDEO;
    					pMedia->ulFileSize = gVidFileInfo.file_size;
    					codec_type = gAVIVideoMedia[ubCurVidIndex].ulCodecType;
    					//bSkipHeader = MMP_FALSE;
					//}
                    m_ulVideoTrackTotalCnt++;
                    gubIsVidDualTrks = (m_ulVideoTrackTotalCnt<2) ? MMP_FALSE : MMP_TRUE;                        
					break;
				case ATOM_AUDS://MKTAG('a', 'u', 'd', 's'):
				    //if (gVidFileInfo.is_audio_available) {
				    //    bSkipHeader = MMP_TRUE;
				    //} 
				    //else {
    					pMedia = &gAVIAudioMedia;
    					gVidFileInfo.is_audio_available = 1;
    					pMedia->ulTrackID = stream_index;
    					pMedia->ulCodecType = CODEC_TYPE_AUDIO;
    					pMedia->ulFileSize = gVidFileInfo.file_size;
    					codec_type = CODEC_TYPE_AUDIO;
    					//bSkipHeader = MMP_FALSE;
					//}
					break;
				case ATOM_TXTS://MKTAG('t', 'x', 't', 's'):
					pMedia = &gAVITextMedia;
					pMedia->ulTrackID = stream_index;
					codec_type = CODEC_TYPE_DATA;
					break;
				case ATOM_DATS://MKTAG('d', 'a', 't', 's'):
					pMedia = &gAVIDataMedia;
					pMedia->ulTrackID = stream_index;
					codec_type = CODEC_TYPE_DATA;
					break;
				default:
					goto fail;
				}
				
				//if(bSkipHeader){
				//    MMPF_AVIPSR_SkipFile(ulSize - 8);
				//	break;
				//}
				
				MMPF_AVIPSR_SkipFile(2*4);  // skip flags(4) priority(2) language(2)
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pMedia->ulInitFrame);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pMedia->ulScale);
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pMedia->ulRate);
				
				if (!(pMedia->ulRate && pMedia->ulScale)){
				    bUndefinedTime = MMP_TRUE;
					if (avih.frame_period){
						pMedia->ulRate = 1000000;
						pMedia->ulScale = avih.frame_period;
					} 
					else {
						pMedia->ulRate = 25;
						pMedia->ulScale = 1;
					}
				}

				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp); // start
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pMedia->ulTotalFrame); // length
				
				if (pMedia->ulTotalFrame == 0) {
				    pMedia->ulTotalFrame = AVI_UNDEFINESAMPLENUM;
				}
				
				pMedia->ulStreamTime = (MMP_ULONG)((MMP_ULONG64)((MMP_ULONG64)((MMP_ULONG64)pMedia->ulTotalFrame * (MMP_ULONG64)pMedia->ulScale) * 1000) / pMedia->ulRate);
				MMPF_AVIPSR_SkipFile(2*4); // skip suggest buffer, quality
				MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pMedia->ulSampleSize);

				if (codec_type == CODEC_TYPE_VIDEO) {
					gVidFileInfo.video_info[ubCurVidIndex].total_video_time = pMedia->ulStreamTime;
					pMedia->ulSampleSize = 0;
				}
				if (codec_type == CODEC_TYPE_AUDIO) {
				    if (pMedia->ulSampleSize > 0) {
				        pMedia->ulStreamTime /= pMedia->ulSampleSize;
				    }
					gVidFileInfo.audio_info.total_audio_time = pMedia->ulStreamTime;
				}

				MMPF_AVIPSR_SkipFile(ulSize - 12*4);
				break;
			case ATOM_STRF://MKTAG('s', 't', 'r', 'f'):
			    //if (bSkipHeader) {
			    //    MMPF_AVIPSR_SkipFile(ulSize);
			    //    break;
			    //}
				
				if (stream_index < 0) {
					MMPF_AVIPSR_SkipFile(ulSize);
				} 
				else {
					switch (codec_type) {
					case CODEC_TYPE_VIDEO:
					{
						MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);
						ulMetaSize = tmp;
						MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);
						gVidFileInfo.video_info[ubCurVidIndex].width = (MMP_USHORT)tmp;
						
						MEMCPY(dest_buf, &tmp, 4);
						MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &tmp);
						gVidFileInfo.video_info[ubCurVidIndex].height = (MMP_USHORT)tmp;
						MEMCPY(dest_buf + 4, &tmp, 4);
						MMPF_AVIPSR_SkipFile(4);  // skip planes , depth
						MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &ulTag1);
						MMPF_AVIPSR_SkipFile(5*4);

                        MMPF_AVIPSR_ToUpper((MMP_UBYTE *)&ulTag1, 4);
						
						gVidFileInfo.video_info[ubCurVidIndex].format = MMPF_AVIPSR_GetCodecType(codec_bmp_tags, ulTag1);//OAmonkey add for codec_bmp_tags
						
						if (gVidFileInfo.video_info[ubCurVidIndex].format == MMP_VID_VIDEO_UNSUPPORTED) {
						    gVidFileInfo.video_info[ubCurVidIndex].format = MMP_VID_VIDEO_UNSUPPORTED;
						    gVidFileInfo.is_video_available[ubCurVidIndex] = 0;
					    }
						// this need to parse video header, now skip
						// end parse video header
						if (gVidFileInfo.video_info[ubCurVidIndex].format == MMP_VID_VIDEO_WMV) {
						    MMPF_VIDPSR_ReadFile(dest_buf + 8, m_ulCurFileOffset, ulMetaSize - 40);
						    ulMetaSize = ulMetaSize - 40 + 8;
						}
						MMPF_AVIPSR_SkipFile(ulSize - 40);
						break;
				    }
					case CODEC_TYPE_AUDIO:
						MMPF_AVIPSR_GetAudioHeader(ulSize);

						if (bUndefinedTime) {
						    gVidFileInfo.audio_info.total_audio_time = 0;
						}
						if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AAC && pMedia->ulSampleSize != 0
						    && pMedia->ulRate == gVidFileInfo.audio_info.sampling_rate){
						    if (gVidFileInfo.audio_info.bitrate != 0) {
						        gVidFileInfo.audio_info.total_audio_time = pMedia->ulStreamTime
    						    = (MMP_ULONG)(((MMP_ULONG64)((MMP_ULONG64)pMedia->ulTotalFrame * 8 * 1000)) / ((MMP_ULONG64)gVidFileInfo.audio_info.bitrate));
						    }
						    pMedia->ulSampleSize = 0;
						} 
						else if ((pMedia->ulRate * 8) == gVidFileInfo.audio_info.bitrate && pMedia->ulSampleSize == 0) {
						    pMedia->ulSampleSize = pMedia->ulScale;
						}
						
						if(ulSize%2)
							MMPF_AVIPSR_SkipFile(1);
						// maybe has extradata
						break;
					default :
						MMPF_AVIPSR_SkipFile(ulSize);
						break;

					}
				}
				break;
                    break;
//====================================================================================
    
		    case ATOM_INDX://MKTAG('i', 'n', 'd', 'x'):
		        MMPF_AVIPSR_SkipFile(4); // Skip PerEntry(2), IndexSubType(1), IndexType(1)
		        MMPF_AVIPSR_GetLittleLong(m_ulCurFileOffset, 4, &pMedia->ulSpIdxNum);
		        pMedia->ulSpIdxCount = 0;
		        MMPF_AVIPSR_SkipFile(16); // Skip ChunkID(4), Reserved bytes(12)
		        pMedia->ulSpIdxOffset = m_ulCurFileOffset;
                //printc(FG_YELLOW("indx data start:%x\r\n"), pMedia->ulSpIdxOffset);                		                    
		        pMedia->bIsSuperIdx = MMP_TRUE;
		        ulSize += (ulSize & 1);
		        MMPF_AVIPSR_SkipFile(ulSize - 24);
		        break;
			case ATOM_IDX1://MKTAG('i', 'd', 'x', '1'):
                pMedia->ulIdx1start = m_ulCurFileOffset - 8;
                //printc(FG_YELLOW("idx1 data start:%x\r\n"), pMedia->ulIdx1start);                
			    MMPF_AVIPSR_SkipFile(ulSize);
			    break;
			default:
				ulSize += (ulSize & 1);
				MMPF_AVIPSR_SkipFile(ulSize);
				//TL: There are many dummy data when SPO happend.
				//Those data size are zero so that "while loop" can't be interrupted.
				if(!ulSize) {
					m_ulCurFileOffset = gVidFileInfo.file_size;
				}				
				break;	
		}
	} // end for(;;)
	
//End_of_Header:

	if (stream_index < 0) {
	fail:
		return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
	}

	pVidMedia->ulMoviOffset = AviCtx.ulMoviList;
	gAVIAudioMedia.ulMoviOffset = AviCtx.ulMoviList;
	pVidMedia->ulIndexMoviOffset = AviCtx.ulMoviList;
	gAVIAudioMedia.ulIndexMoviOffset = AviCtx.ulMoviList;
	pVidMedia->ulMoviEnd = AviCtx.ulMoviEnd;
	gAVIAudioMedia.ulMoviEnd = AviCtx.ulMoviEnd;
	#if (SPEED_UP_AVI_OPEN)
	#else
	MMPF_AVIPSR_SetIndex(AviCtx.ulMoviList, AviCtx.ulMoviEnd, gVidFileInfo.file_size, gAVIVideoMedia.ulTrackID);
	MMPF_AVIPSR_SetIndex(AviCtx.ulMoviList, AviCtx.ulMoviEnd, gVidFileInfo.file_size, gAVIAudioMedia.ulTrackID);
	#endif
	pVidMedia->bSeekEnable = MMP_FALSE;
	gAVIAudioMedia.bSeekEnable = MMP_FALSE;
	
    #if (SPEED_UP_AVI_OPEN)
    if(AviCtx.ulHaveIndex || pVidMedia->bIsSuperIdx) {
        gVidFileInfo.is_video_seekable[m_ubVideoCurTrack] = MMP_TRUE;
    }
    #else
    if(pVidMedia->ulIdxOffset || pVidMedia->bIsSuperIdx) {
        gVidFileInfo.is_video_seekable[m_ubVideoCurTrack] = MMP_TRUE;
    }
    #endif

    pVidSample->ulSize = 0;
    m_bCheckUnknownChunk = MMP_TRUE;
    
    #if (SPEED_UP_AVI_OPEN)
    lFilePos = pVidMedia->ulIdxOffset;
    #endif
    
    while (pVidSample->ulSize == 0) {
        
        #if (SPEED_UP_AVI_OPEN)

    	error = MMPF_AVIPSR_GetNextSample(pVidMedia, pVidSample, 1);

    	if (error != MMP_ERR_NONE && pVidSample->ulSize == 0) {
    	    pVidInfo->format = MMP_VID_VIDEO_NONE;
    	    gVidFileInfo.is_video_available[m_ubVideoCurTrack] = 0;
    	    m_bCheckUnknownChunk = MMP_FALSE;
    	    return MMP_VID_UNSUPPORTED_VIDEO;
    	}
    	
    	if (pVidSample->ulSize != 0) {
        	pVidMedia->ulIdxOffset = lFilePos;
        	if(pVidMedia->ulIdxOffset == 0){
        	    pVidMedia->ulSamplePos = 0;
        	}
    	}
        #else
        
        if (pVidMedia->ulIdxOffset && (pVidMedia->bIsSuperIdx == MMP_FALSE)){

        	lFilePos = pVidMedia->ulIdxOffset;
        	error = MMPF_AVIPSR_GetNextSample(&pVidMedia, &pVidSample, 1);
        	
        	if(error != MMP_ERR_NONE && pVidSample->ulSize == 0) {
        	    pVidInfo->format = MMP_VID_VIDEO_NONE;
        	    gVidFileInfo.is_video_available[0] = 0;
        	    break;
        	}
        	if(pVidSample->ulSize != 0) {
            	pVidMedia->ulIdxOffset = lFilePos;
            	if(pVidMedia->ulIdxOffset == 0){
            	    pVidMedia->ulSamplePos = 0;
            	}
        	}
        } 
        else {
            if (pVidMedia->bIsSuperIdx) {
                error = MMPF_AVIPSR_GetNextSample(&pVidMedia, &pVidSample, 1);
                
                if (error != MMP_ERR_NONE && pVidSample->ulSize == 0) {
            	    pVidInfo->format = MMP_VID_VIDEO_NONE;
            	    gVidFileInfo.is_video_available[0] = 0;
            	    break;
            	}
                
                if (pVidSample->ulSize != 0) {
                    pVidMedia->ulIdxOffset -= 8;
                    pVidMedia->ulReadCount -= 8;
                    pVidMedia->ulStdIdxCount--;
                }
            } 
            else {
                lFilePos = pVidMedia->ulIdxOffset;
            	error = MMPF_AVIPSR_GetNextSample(&pVidMedia, &pVidSample, 1);
            	
            	if (error != MMP_ERR_NONE && pVidSample->ulSize == 0) {
            	    pVidInfo->format = MMP_VID_VIDEO_NONE;
            	    gVidFileInfo.is_video_available[0] = 0;
            	    break;
            	}
            	if (pVidSample->ulSize != 0) {
                	pVidMedia->ulIdxOffset = lFilePos;
                	if (pVidMedia->ulIdxOffset == 0) {
                	    pVidMedia->ulSamplePos = 0;
                	}
            	}
            }
        }
        #endif
    }
    
    m_bCheckUnknownChunk = MMP_FALSE;
    pVidMedia->ulFrameIndex = 0;
    
AVI_INIT :
    if (bVideoInitFail) 
    {
        MMP_ULONG ulTempFrame = 0;
        
        #if (SPEED_UP_AVI_OPEN)
        if (pVidMedia->ulIdxOffset == 0 && !(pVidMedia->bIsSuperIdx)) {
            MMPF_AVIPSR_SetIndex(pVidMedia->ulMoviOffset, pVidMedia->ulMoviEnd, gVidFileInfo.file_size, pVidMedia->ulTrackID);
            lFilePos = pVidMedia->ulIdxOffset;
        }
        if (pVidMedia->ulIdxOffset == 0 && !(pVidMedia->bIsSuperIdx)) {
            return MMP_VID_UNSUPPORTED_FILE;
        }
        pVidMedia->bSeekEnable = MMP_TRUE;
        #endif
        
        error = MMPF_AVIPSR_SeekIFrame(1, &ulTempFrame);
        
        if (error != MMP_VIDPSR_ERR_EOS && error != MMP_ERR_NONE) {
            return MMP_VID_UNSUPPORTED_FILE;
        }
        if (error == MMP_VIDPSR_ERR_EOS) {
            return MMP_VID_UNSUPPORTED_FILE;
        }
        if (pVidMedia->ulFrameIndex > AVI_MAX_SEARCH_SAMPLE_COUNT) {
            return MMP_VID_UNSUPPORTED_FILE;
        }
    }
    
    if (bInitCodec && gVidFileInfo.is_video_available[m_ubVideoCurTrack]) 
    {
    	if (pVidInfo->format == MMP_VID_VIDEO_MP4V) {
    	
            MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_CACHE);

    	    MMPF_VIDPSR_ReadFile(dest_buf, pVidSample->ulOffset, pVidSample->ulSize);
    	    
    	    #if (AIT_DIVXDRM_SUPPORT == 1)
            if (DRM_DIVX_IsEnable()) {
                // discript, video data adress : dest_buf, size : gAVIVideoSample.ulSize, key : m_VideoKey
                DRM_DIVX_DecryptVideo(dest_buf, gAVIVideoSample.ulSize);
            }
            #endif

    	    pVidInfo->width  = 0;
            pVidInfo->height = 0;
    	    gVidFileInfo.is_video_available[0] = 0;
    	} 
    	else if (pVidInfo->format == MMP_VID_VIDEO_H263 || pVidInfo->format == MMP_VID_VIDEO_DIVX){
    	    gVidFileInfo.is_video_available[0] = 0;
    	} 
    	else if (pVidInfo->format == MMP_VID_VIDEO_H264) {
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_CACHE);
    	    MMPF_VIDPSR_ReadFile(dest_buf, pVidSample->ulOffset, pVidSample->ulSize);
    	    
    	    #if (AIT_DIVXDRM_SUPPORT == 1)
            if (DRM_DIVX_IsEnable()) {
                // discript, video data adress : dest_buf, size : gAVIVideoSample.ulSize, key : m_VideoKey
                DRM_DIVX_DecryptVideo(dest_buf, pVidSample->ulSize);
            }
            #endif
    	    
    	    pVidInfo->width  = 0;
            pVidInfo->height = 0;
            
    	    //#if (VIDEO_P_EN)
    	    {
        	    extern MMP_HANDLE          *hVideo;
                extern MMPF_VIDEO_FUNC     m_vcodec;
                
        	    hVideo = MMPF_VIDEO_Open(CODEC_H264, &m_vcodec);
        	    
        	    m_vcodec.MMPF_VIDDEC_ConnectBSBuf(hVideo, dest_buf, pVidSample->ulSize, MMP_MEDIA_NEAR_RING_BUFFER);
                
                ubVideoCurTrackTmp = m_ubVideoCurTrack;
                //m_ubVideoCurTrack = ubCurVidIndex;        	      
                if (MMP_ERR_NONE != 
                    m_vcodec.MMPF_VIDDEC_Init(hVideo,dest_buf, pVidSample->ulSize,
                                              pVidInfo->width,
                                              pVidInfo->height,2)) {                                  
                               
                    DBG_S(0,"Init video failed\r\n");
                    bVideoInitFail = MMP_TRUE;
                    goto AVI_INIT;
                 }
                 m_ubVideoCurTrack = ubVideoCurTrackTmp;
                
                 m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,pVidInfo);
                 
                 pVidInfo->width = pVidInfo->width - pVidInfo->cropping_left - pVidInfo->cropping_right;
                                                        
                 pVidInfo->height = pVidInfo->height - pVidInfo->cropping_top - pVidInfo->cropping_bottom;                                                        		
            }
    	    //#else
    	    //#endif
    	}
    	else if (pVidInfo->format == MMP_VID_VIDEO_WMV) {

    	    gVidFileInfo.is_video_available[0] = 0;
    	}
    	else if(pVidInfo->format == MMP_VID_VIDEO_MJPG){
            
            m_MFDFormat = MMP_VID_VIDEO_MJPG;
                       
    	    MMPF_VIDPSR_SwapDepackCacheIdx(AVI_VIDEO_CACHE);
    	    MMPF_VIDPSR_ReadFile(dest_buf, pVidSample->ulOffset, pVidSample->ulSize);    
    	    
    	    #if (VIDEO_P_EN)
    	    {
        	    extern MMP_HANDLE          *hVideo;
                extern MMPF_VIDEO_FUNC     m_vcodec;
                extern MMP_ULONG m_ulJpgSramAddr;
                
                hVideo = MMPF_VIDEO_Open(CODEC_MJPG, &m_vcodec);
      
                if (MMP_ERR_NONE !=
                    m_vcodec.MMPF_VIDDEC_Init(hVideo,dest_buf, pVidSample->ulSize,
                                              pVidInfo->width,
                                              pVidInfo->height,2)) {                                  
                               
                    DBG_S(0,"Init video failed\r\n");
                    bVideoInitFail = MMP_TRUE;
                    goto AVI_INIT;
                 }

                 m_vcodec.MMPF_VIDDEC_ConnectBSBuf(hVideo, dest_buf, pVidSample->ulSize, m_ulJpgSramAddr);

                 m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,pVidInfo);
            }
    	    #endif    	   
        }
    	else {
    	    gVidFileInfo.is_video_available[0] = 0;
    	}
	}
	
	if (bVideoInitFail) {
	    pVidMedia->ulFrameIndex = 0;
	    pVidMedia->ulIdxOffset = lFilePos;
    	pVidMedia->ulSamplePos = 0;
        #if (SPEED_UP_AVI_OPEN)
        pVidMedia->bSeekEnable = MMP_FALSE;
        #endif
	}
	
	if (gVidFileInfo.is_video_available[m_ubVideoCurTrack]) {
	    if (pVidMedia->ulScale == 0) {
	        return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
	    }
	    if ((avih.bit_rate < gVidFileInfo.audio_info.bitrate || avih.bit_rate == 0) && pVidInfo->total_video_time != 0) {
	         avih.bit_rate = (gVidFileInfo.file_size / (pVidInfo->total_video_time / 1000)) * 8;
	    }
	    pVidInfo->bitrate = avih.bit_rate - gVidFileInfo.audio_info.bitrate;
	    pVidInfo->fps = pVidMedia->ulRate / pVidMedia->ulScale;
	    gVidFileInfo.videoTrackNum = m_ulVideoTrackTotalCnt; /// Hard code to 1 track for AVI file now. If need to support multi track. Need to modify.
	}
	
	// Set audio info
	if (gVidFileInfo.is_audio_available) {
        if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AAC_LC) {
            mp4ASC.samplingFrequency = gVidFileInfo.audio_info.sampling_rate;
            if (mp4ASC.objectTypeIndex == 0)
                mp4ASC.objectTypeIndex = 2;
        }
        #if (VWAV_P_EN)
        else if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_WAV) {
            WAVFileInfo *wavinfo;
            MMPF_GetWAVInfo(&wavinfo);
            wavinfo->bitRate = gVidFileInfo.audio_info.bitrate;
            wavinfo->totaldataSize = 0;
            wavinfo->compressionCode = 0;
            wavinfo->nChannels = gVidFileInfo.audio_info.channel_nums;
            wavinfo->sampleRate = gVidFileInfo.audio_info.sampling_rate;
            wavinfo->newSampleRate = gVidFileInfo.audio_info.sampling_rate;
            wavinfo->bitsPerSample = gVidFileInfo.audio_info.bits_per_coded_sample;
            DBG_S(3, "Set WAV Info Done\r\n");
        }
        #endif
        
        #if (VMP3_P_EN)
        if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_MP3) {
            MMPF_SetMP3TimeHandle(gVidFileInfo.audio_info.bitrate);
        }
        #endif
        
        if ((gAVIAudioMedia.ulRate == 0) || (gAVIAudioMedia.ulScale == 0)) {
            return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
        }
        
        m_ulAudSampleRate = gVidFileInfo.audio_info.sampling_rate;
	}
	
	if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_UNSUPPORTED || gVidFileInfo.is_audio_available == 0) {
	    gVidFileInfo.total_file_time = pVidInfo->total_video_time;
	} 
	else {
    	if (pVidInfo->total_video_time > gVidFileInfo.audio_info.total_audio_time) {
        	gVidFileInfo.total_file_time = pVidInfo->total_video_time;
        } 
        else {
        	gVidFileInfo.total_file_time = gVidFileInfo.audio_info.total_audio_time;
        }
    }
	
    #if (SPEED_UP_AVI_SEEK)
    m_ulPreAudioSize = 0;
    m_ulPreVideoChunk = 0;
    m_bCountPreAudio = MMP_FALSE;
    m_bCountPreVideo = MMP_FALSE;
    if (gVidFileInfo.is_audio_available && 
        gVidFileInfo.is_video_available[m_ubVideoCurTrack]) 
	{
        m_bCountPreAudio = MMP_TRUE;
        m_bCountPreVideo = MMP_TRUE;
    }
	#endif
	
	#if (SPEED_UP_AVI_SEEK)
    if (bInitCodec)
    {
	    memcpy(&gIDXVideoMedia, &gAVIVideoMedia[m_ubVideoCurTrack], sizeof(MMP_AVI_MEDIA_INFO));
	    memcpy(&gIDXAudioMedia, &gAVIAudioMedia, sizeof(MMP_AVI_MEDIA_INFO));
		MMPF_IDXPSR_SetCacheBuf(m_ulFastSeekBufStart, FASTSEEK_BUFFER_SIZE);
	    MMPF_IDXPSR_Initialize((MMP_ULONG)m_sDepackCache[INIT_CACHE].fileHandle, gVidFileInfo.file_size);
		MMPF_AVIPSR_InitIDXPSRContext(gVidFileInfo.file_size, ulIDXVBuffer, ulIDXABuffer, AVI_INDEX_BUFFER_SIZE, gVidFileInfo.total_file_time);
		
		#if (SPEED_UP_AVI_OPEN)
	    if(pVidMedia->ulIdxOffset == 0 && !(pVidMedia->bIsSuperIdx)) {
	        MMPF_AVIPSR_SetIndex(pVidMedia->ulMoviOffset, pVidMedia->ulMoviEnd, gVidFileInfo.file_size, pVidMedia->ulTrackID);
	    }
	    if(gAVIAudioMedia.ulIdxOffset == 0 && !(gAVIAudioMedia.bIsSuperIdx)) {
	        MMPF_AVIPSR_SetIndex(gAVIAudioMedia.ulMoviOffset, gAVIAudioMedia.ulMoviEnd, gVidFileInfo.file_size, gAVIAudioMedia.ulTrackID);
	    }
	    #endif
		MMPF_AVIPSR_BuildIndexTable();
		#if (VIDEO_BGPARSER_EN == 0)
		//MMPF_AVIPSR_GetFileTime(&gVidFileInfo.total_file_time, (MMP_ULONG *)&gVidFileInfo.video_info.total_video_time, &gVidFileInfo.audio_info.total_audio_time);
		#endif
    }
	#endif

    if (pInfo) {
        MEMCPY(pInfo, &gVidFileInfo, sizeof(MMP_VIDEO_CONTAINER_INFO));
    }	
	
	return 0;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_AVIPSR_Parse
//  Description :
//------------------------------------------------------------------------------
MMP_VID_FILEVALIDATION MMPF_AVIPSR_Parse(void)
{
	return MMPF_AVIPSR_ParseEx(NULL, MMP_TRUE);
}

#if (SUPPORT_NO_IFLAG_SEEK == 1)
//------------------------------------------------------------------------------
//  Function    : MMPF_AVIPSR_SupportNoIfraflagSeek
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_AVIPSR_SupportNoIfraflagSeek(MMP_ULONG ulIdx, MMP_ULONG ulTrackNo, MMP_ULONG ulLen, MMP_ULONG *ulFlags)
{
    MMP_ERR err = MMP_ERR_NONE;
    //Notic: I-frame search can miss but can't wrong. In others, if search wrong I as P, decoder will be hanged.
    if (!gulIframeSize[ulTrackNo]) {
        gulIframeSize[ulTrackNo] = ulLen;
        if (*ulFlags & AVIIF_KEYFRAME) {
            gubIdx1HasIflag[ulTrackNo] = MMP_TRUE;                   
        }
        else {
            *ulFlags = AVIIF_KEYFRAME;            
            gubIdx1HasIflag[ulTrackNo] = MMP_FALSE;
            printc(FG_RED("FileOfAvi No I Flag -> Feature not finish yet!\r\n"));                        
            printc(FG_RED("Tk%x idx=%d ISize:%x lim:%x\r\n"), ulTrackNo, (ulIdx+1), ulLen, ((gulIframeSize[ulTrackNo]*63)>>6));            
            err = MMP_VIDPSR_ERR_NOT_IMPLEMENTED;
        }
    }
    
    if (!gubIdx1HasIflag[ulTrackNo] && (ulLen > gulIframeSize[ulTrackNo])) {
        //if (!(flags & AVIIF_KEYFRAME)) {
        //    printc(FG_RED("idx:%d len:%x\r\n"), (ulIdx+1), ulLen);
        //}
        //else {
        //    printc(FG_GREEN("idx:%d len:%x\r\n"), (ulIdx+1), ulLen);
        //}
        *ulFlags = AVIIF_KEYFRAME;
    }
    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_AVIPSR_ChkFrameIfHasIflag
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_AVIPSR_ChkFrameIfHasIflag(MMP_AVI_SAMPLE_INFO *pSample, MMP_ULONG *ulFlags)
{
    MMP_ERR err = MMP_ERR_NONE;
	MMP_UBYTE bChkIfraTmp[8];
    MMP_UBYTE *ptr = (MMP_UBYTE *)&bChkIfraTmp[0];
    MMP_UBYTE gubNalUnitType;
    MMP_ULONG ulSkipCnt;
    if(!gubIdx1HasIflag[m_ubVideoCurTrack] && (pSample->ulSize > gulIframeSize[m_ubVideoCurTrack])) {
        MMPF_VIDPSR_ReadFile(ptr, pSample->ulOffset, sizeof(bChkIfraTmp));
        ulSkipCnt = -1;
        do {     
            ulSkipCnt++;                        
        } while(*(ptr + ulSkipCnt) == 0);
        ulSkipCnt++;
        gubNalUnitType = *(ptr+ulSkipCnt) & 0x1F;
#if (DEBUG_VIDEO & LOG_PARSE)
        printc(FG_YELLOW("Cnt[%x] Symb[%x]\r\n"), ulSkipCnt, *(ptr+ulSkipCnt)); //0x61, 0x65 and 0x67
#endif
        if (gubNalUnitType == 0x1 || gubNalUnitType == 0x5 || gubNalUnitType == 0x7 || gubNalUnitType == 0x8) {
            *ulFlags = AVIIF_KEYFRAME;
        }
        else {
            printc(FG_RED("Risk Symb[%x %x %x %x %x]\r\n"), *(ptr+0), *(ptr+1),*(ptr+2), *(ptr+3), *(ptr+4));
            err = MMP_VIDPSR_ERR_PARAMETER;
        }
    }
    return err;
}
#endif //#if (SUPPORT_NO_IFLAG_SEEK == 1)

#if (DEBUG_VIDEO & DUMP_BS_BUFDATA)
#include "ait_utility.h"
//------------------------------------------------------------------------------
//  Function    : MMPF_AVIPSR_DumpCompBuf
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_AVIPSR_DumpCompBuf(MMP_ULONG ulAddr,MMP_ULONG ulSize)
{
	MMP_USHORT  fs_err;
    MMP_ULONG   ulRet;
#if 0   
    MMP_BYTE   ubPath[] = "SD:\\BS\\";
    MMP_BYTE   ubFrameNum[] = "XXXX";
    MMP_BYTE   ubSTSZ[] = "XXXXXXXX";
    MMP_BYTE   ubOffset[] = "XXXXXXXX";    
    MMP_BYTE   ubExtension[] = ".yuv";
    MMP_BYTE   ubFilePath[128];    

    MMPC_Int2Str(WRITE_HEADER->ulFrameNum, ubFrameNum);
    MMPC_Int2Str(WRITE_HEADER->ulBSSize, ubSTSZ);    
    MMPC_Int2Str(gAVIVideoSample[m_ubVideoDmxTrackNo].ulOffset, ubOffset);        
    STRCAT(ubFilePath, ubPath);
    STRCAT(ubFilePath, ubFrameNum);
    STRCAT(ubFilePath, "_");
    STRCAT(ubFilePath, ubSTSZ);    
    STRCAT(ubFilePath, "_");
    STRCAT(ubFilePath, ubOffset);     
    STRCAT(ubFilePath, ubExtension);
    
    MMPF_FS_FOpen((MMP_BYTE *)ubFilePath, "w+b", &gulVidFileId);    
    MMPF_FS_FWrite(gulVidFileId, (void *)ulAddr, ulSize, &ulRet);
    MMPF_FS_FClose(gulVidFileId);
#else
    MMP_BYTE   ubPath[] = "SD:\\BS\\DUMPBSBF_TK1.yuv";
    if (!m_bOpenFileOnetime) 
	{
		MMPF_FS_FOpen((const MMP_BYTE *)ubPath, "w+b", &gulVidFileId);    
        m_bOpenFileOnetime = MMP_TRUE;
    }
    fs_err = MMPF_FS_FWrite(gulVidFileId, (void *)ulAddr, ulSize, &ulRet);
    if (m_bVideoEOS) 
    {
        fs_err = MMPF_FS_FClose(gulVidFileId);
    }
#endif    
	return fs_err;
}
#endif //#if (DEBUG_VIDEO & DUMP_BS_BUFDATA)

#endif

#endif
#endif

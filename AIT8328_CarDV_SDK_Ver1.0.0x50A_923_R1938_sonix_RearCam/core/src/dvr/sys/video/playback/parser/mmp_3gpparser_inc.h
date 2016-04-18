/**
 @file mmp_3gpparser_inc.h
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

#ifndef _MMP_3GPPARSER_INC_H_
#define _MMP_3GPPARSER_INC_H_

#if (VIDEO_P_EN)
#include "mmp_3gpparser.h"
#include "mmpf_system.h"
#if (VWAV_P_EN)
#include "mmpf_wavdecapi.h"
#include "adpcmdec_api.h"
#endif

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define ATOM_FTYP     MAKE_TAG_BE('f','t','y','p') 	///< QuickTime Prefs file types [P:NULL, C:NULL]
#define ATOM_MDAT     MAKE_TAG_BE('m','d','a','t') 	///< Media Data Atom [P:NULL, C:Chunk]
#define ATOM_IODS     MAKE_TAG_BE('i','o','d','s') 	///< Initial Object Description Atom
#define ATOM_VMHD     MAKE_TAG_BE('v','m','h','d') 	///< Video Media Info Atom [P:minf, C:NULL]
#define ATOM_SMHD     MAKE_TAG_BE('s','m','h','d') 	///< Sound Media Header Atom [P:minf, C:NULL]
#define ATOM_DINF     MAKE_TAG_BE('d','i','n','f') 	///< Data Information Atom [P:minf, C:dref]
#define ATOM_MVHD     MAKE_TAG_BE('m','v','h','d') 	///< Movie header Atom  [P:moov, C:NULL]
#define ATOM_MOOV     MAKE_TAG_BE('m','o','o','v') 	///< Movie Atom [P:NULL, C:mvhd, trak]
#define ATOM_MDIA     MAKE_TAG_BE('m','d','i','a')	///< Media Atom [P:trak, C:mdhd, hdlr, minf]
#define ATOM_TRAK     MAKE_TAG_BE('t','r','a','k') 	///< Track Atom. [P:moov, C:tkhd, mdia]
#define ATOM_TKHD     MAKE_TAG_BE('t','k','h','d') 	///< Track Header Atom. [P:trak, C:NULL]
#define ATOM_MINF     MAKE_TAG_BE('m','i','n','f') 	///< Media Info. [P:mdia C:vmhd, dinf, stbl]
#define ATOM_SOUN     MAKE_TAG_BE('s','o','u','n')
#define ATOM_VIDE     MAKE_TAG_BE('v','i','d','e')
#define ATOM_MDHD     MAKE_TAG_BE('m','d','h','d') 	///< Media Header Atom. [P:mdia C:NULL]
#define ATOM_HDLR     MAKE_TAG_BE('h','d','l','r') 	///< Media Handler Reference Atom. [P:mdia C:NULL]
#define ATOM_STBL     MAKE_TAG_BE('s','t','b','l') 	///< Sample Table Atom [P:minf C:stsd, stts, stsc, stsz, stco, stss]
#define ATOM_STSD     MAKE_TAG_BE('s','t','s','d') 	///< Sample Description Atom [P:stbl, C:NULL]
#define ATOM_STTS     MAKE_TAG_BE('s','t','t','s') 	///< Time to Sample Atom [P:stbl, C:NULL]
#define ATOM_STSC     MAKE_TAG_BE('s','t','s','c')	///< Sample to Chunk Atom [P:stbl, C:NULL]
#define ATOM_STSZ     MAKE_TAG_BE('s','t','s','z') 	///< Sample Size Atom [P:stbl, C:NULL]
#define ATOM_STCO     MAKE_TAG_BE('s','t','c','o') 	///< Chunk Offset Atom [P:stbl, C:NULL]
#define ATOM_STSS     MAKE_TAG_BE('s','t','s','s') 	///< Sync Sample Atom [P:stbl, C:NULL] contains video keyframe
#define ATOM_CTTS     MAKE_TAG_BE('c','t','t','s') 	///< Composition time to sample atom [P:stbl, C:NULL]
#define ATOM_WAVE     MAKE_TAG_BE('w','a','v','e')
#define ATOM_ESDS     MAKE_TAG_BE('e','s','d','s')
#define ATOM_AVCC     MAKE_TAG_BE('a','v','c','C')
#define ATOM_EDTS     MAKE_TAG_BE('e','d','t','s')
#define ATOM_ELST     MAKE_TAG_BE('e','l','s','t')
#define ATOM_UDTA     MAKE_TAG_BE('u','d','t','a')
#define ATOM_MJPA     MAKE_TAG_BE('m','j','p','a')
#define ATOM_SKIP     MAKE_TAG_BE('s','k','i','p')  ///< Skip atom
#define ATOM_PANA     MAKE_TAG_BE('P','A','N','A')  ///< PANA atom

#define OBJTYPE_MP2   		0x69
#define OBJTYPE_MP3   		0x6B
#define OBJTYPE_AC3   		0xA5
#define OBJTYPE_AAC   		0x40
#define OBJTYPE_AAC_M 		0x66   // AAC Main
#define OBJTYPE_AAC_L 		0x67   // AAC Low
#define OBJTYPE_AAC_S 		0x68   // AAC SSR

#define STSZ_BUFFER_SIZE 	27               		///< Number of STSZ entries to keep in the memory.
#define STSC_BUFFER_SIZE 	15                 		///< Number of STSC entries to keep in the memory.
#define STCO_BUFFER_SIZE 	32               		///< Number of STCO entries to keep in the memory.
#define STTS_BUFFER_SIZE 	7                		///< Number of STTS entries to keep in the memory.
#define STSS_BUFFER_SIZE 	16                		///< Number of STSS entries to keep in the memory.
#define CTTS_BUFFER_SIZE 	17						///< Number of CTSS entries to keep in the memory.

#define MAX_VID_TK         	(2) 					///< Support maxium Video truck
#define MAX_AUD_TK   	    (1) 					///< Support maxium Audio truck
#define MAX_TRACK_NUM   	(MAX_AUD_TK+MAX_VID_TK) ///< Support maxium truck
#define MAX_DEPACK_CACHE 	(MAX_TRACK_NUM * 7) 	///< Use 14 depack caches	
#define CACHE_POS_MASK 		(DEPACK_CACHE_SIZE-1)	///< Cache mask to get the position in the page
#define CACHE_PAGE_MASK 	(~CACHE_POS_MASK)   	///< Cache mask to get the file page number

#define FASTSEEK_BUFFER_SIZE 	(256*1024)

#define VIDPLAY_LOCK_MUTEX_TIME	(100000)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

/** @brief Atom type in 3GP

These atoms are in sample table (stbl)
*/
typedef enum _MMPF_3GP_ATOM {
    MMPF_3GP_ATOM_STSZ = 0, 	///< Sample size table
    MMPF_3GP_ATOM_STSC,     	///< Sample to chunk table
    MMPF_3GP_ATOM_STCO,     	///< Chunk offset table
    MMPF_3GP_ATOM_STTS,     	///< Time to sample table
    MMPF_3GP_ATOM_STSS,     	///< Sync sample table. Suppose only video has this table
    MMPF_3GP_ATOM_CTTS,			///< Composition Time to Sample table
	MMPF_3GP_ATOM_MAX
} MMPF_3GP_ATOM;
#if (MAX_VID_TK == 1)
/** @brief Track type in 3GP file
*/
typedef enum _MMPF_3GP_TRACK {
    MMPF_3GP_TRACK_AUDIO = 0, 	///< Audio track
    MMPF_3GP_TRACK_VIDEO,     	///< Video track
    MMPF_3GP_TRACK_HINT,      	///< Hint track
    MMPF_3GP_TRACK_UNDEFINED 	///< Undefined track
} MMPF_3GP_TRACK;

/** Used as index of 3gpp depack cache structure
*/
typedef enum _MMPF_3GP_CACHE_INDEX {
	VIDEO_CACHE = 0,         	///< Video data depack buffer index
	AUDIO_CACHE,              	///< Audio data depack buffer index
	VIDEO_CHUNK_CACHE,      	///< Video atom depack buffer index
	AUDIO_CHUNK_CACHE,      	///< Audio atom depack buffer index, STSZ
	VIDEO_STSC_CACHE,
	AUDIO_STSC_CACHE,
	VIDEO_STCO_CACHE,
	AUDIO_STCO_CACHE,
	VIDEO_STTS_CACHE,
	AUDIO_STTS_CACHE,
	VIDEO_STSS_CACHE,
	AUDIO_STSS_CACHE,
	VIDEO_CTTS_CACHE,
	AUDIO_CTTS_CACHE
} MMPF_3GP_CACHE_INDEX;
#else
/** @brief Track type in 3GP file
*/
typedef enum _MMPF_3GP_TRACK {
    MMPF_3GP_TRACK_AUDIO = 0, 	///< Audio track
    MMPF_3GP_TRACK_VIDEO_TK0,   ///< Video track 0
    MMPF_3GP_TRACK_VIDEO_TK1,   ///< Video track 1 
    MMPF_3GP_TRACK_HINT,      	///< Hint track
    MMPF_3GP_TRACK_UNDEFINED 	///< Undefined track
} MMPF_3GP_TRACK;

/** Used as index of 3gpp depack cache structure
*/
typedef enum _MMPF_3GP_CACHE_INDEX {
	VIDEO_CACHE_TK0 = 0,         	///< Video data depack buffer index
	VIDEO_CACHE_TK1,             	///< 	
	AUDIO_CACHE,                  	///< Audio data depack buffer index
	VIDEO_CHUNK_CACHE_TK0,          ///< Video atom depack buffer index
	VIDEO_CHUNK_CACHE_TK1,      	///< 	
	AUDIO_CHUNK_CACHE,      	    ///< Audio atom depack buffer index, STSZ
	VIDEO_STSC_CACHE_TK0,
	VIDEO_STSC_CACHE_TK1,	
	AUDIO_STSC_CACHE,
	VIDEO_STCO_CACHE_TK0,
	VIDEO_STCO_CACHE_TK1,	
	AUDIO_STCO_CACHE,
	VIDEO_STTS_CACHE_TK0,
	VIDEO_STTS_CACHE_TK1,	
	AUDIO_STTS_CACHE,
	VIDEO_STSS_CACHE_TK0,
	VIDEO_STSS_CACHE_TK1,	
	AUDIO_STSS_CACHE,
	VIDEO_CTTS_CACHE_TK0,
	VIDEO_CTTS_CACHE_TK1,	
	AUDIO_CTTS_CACHE
} MMPF_3GP_CACHE_INDEX;
#endif
//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

/** @brief Meta information for 3GP file

index 0 for video and 1 for audio
1. moov\\trak (vide)\\mdia\\minf\\stbl and,
2. moov\\trak (soun)\\mdia\\minf\\stbl
*/
typedef struct _VIDEO_ATOM_BUFFER
{
    MMP_UBYTE ubSize[MMPF_3GP_ATOM_MAX];                      	///< Size of the atom buffer
    MMP_ULONG ulIndex[MAX_TRACK_NUM][MMPF_3GP_ATOM_MAX];               		///< The page index
    MMP_ULONG *ulTableAddress[MAX_TRACK_NUM][MMPF_3GP_ATOM_MAX];        	///< The addresses to the table
    MMP_ULONG ulSizeTable[MAX_TRACK_NUM][STSZ_BUFFER_SIZE];                 ///< Sample size table
    MMP_ULONG ulSamplePerChunkTable[MAX_TRACK_NUM][STSC_BUFFER_SIZE * 3];   ///< Sample per chunk table (3 double words per atom)
    MMP_ULONG ulChunkOffsetTable[MAX_TRACK_NUM][STCO_BUFFER_SIZE];          ///< Chunk offset table
    MMP_ULONG ulSampleTimeTable[MAX_TRACK_NUM][STTS_BUFFER_SIZE * 2];       ///< Sample to time table (2 double words per atom)
    MMP_ULONG ulVideoSyncTable[STSS_BUFFER_SIZE];               ///< Sync sample table
    MMP_ULONG ulCompositionTimeTable[CTTS_BUFFER_SIZE * 2];		///< Composition time table (2 double words per atom)
} VIDEO_ATOM_BUFFER;

/** @brief Private data of the 3GP parser
*/
typedef struct MMPF_3GPPSR_PRIVATEDATA  
{
    MMP_BOOL                bFilling;               ///< If the parser is filling a frame
    MMP_BOOL                bLargeFrame;            ///< If current frame is a large frame
    MMP_BOOL                bFillBufferDone;        ///< If the demuxer is finished and the parser status is not updated.
    MMPF_M_FRAME_INFO    	pPreWriteHeader;       	///< Previous write buffer header
    MMPF_M_FRAME_INFO    	curWriteHeader;         ///< Current write buffer header
    MMP_USHORT              usEOFPadding;           /**< The padding data after the end of a frame. It might includes ARM padding or
                                                         several 0 bytes for padding */
    MMP_USHORT              usBSOffset;             /**< The offset before the bitstream and the starting address of the header.
                                                         It might include LARGE-DMA SD card 8 byte alignment with 512 bytes offset
                                                         and the header itself.*/
    MMP_VID_AUDIOTYPE       sSupportedAudioFmt; 	///< The audio format supported in current firmware
    MMP_VID_VIDEOTYPE       sSupportedVideoFmt; 	///< The video format supported in current firmware
} MMPF_3GPPSR_PRIVATEDATA;

/** Structure of depack buffer information
*/
typedef struct _MMPF_3GPPSR_DEPACKCACHE 
{
	MMP_UBYTE 	*ubAddress;                   		///< Memory address of the depack buffer
	MMP_ULONG64 ulPrePagePos;                 		///< The position of the last loaded page in the file
	MMP_ULONG64 ulPreSeekPos;                     	///< The position of the previous access in the file
    void 		*fileHandle;                    	///< File Handle
} MMPF_3GPPSR_DEPACKCACHE;

#if (EN_SPEED_UP_VID & CACHE_HDL)
/** Structure of Atom parsing information
*/
typedef struct _MMPF_3GPPSR_ATOM_SEQ_INFO 
{
    MMP_ULONG               ulPrevAtom;
    MMP_ULONG               ulCurAtom;
} MMPF_3GPPSR_ATOM_SEQ_INFO;
#endif
//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

MMPF_PSR_FUNC   				m_Parser;	 		///< Parser Function
MMP_PSR_PARSERTYPE 				m_ParserType;		///< Parser Type

MMPF_3GPP_SEEKSTATUS            m_seek[2];				///< Seek Status
mp4AudioSpecificConfig          mp4ASC;             ///< Import mp4ASC in audio structure
MMP_ULONG       				m_ulAudSampleRate;	///< Audio Sample Rate

MMP_UBYTE                       gDemuxOption;
MMP_BOOL                        m_bVideoEOS		= MMP_FALSE;
MMP_BOOL                        m_bAudioEOS		= MMP_FALSE;

static VIDEO_ATOM_BUFFER        m_3GPAtomBuf 	= {	STSZ_BUFFER_SIZE, 
                                                	STSC_BUFFER_SIZE, 
                                                	STCO_BUFFER_SIZE,
                                                	STTS_BUFFER_SIZE,
                                                	STSS_BUFFER_SIZE,
                                                	CTTS_BUFFER_SIZE};
                                                
static MMP_ULONG                m_ulPreAudioOffset 		= 0;
static MMP_ULONG                m_ulPreAudioReduntSize 	= 0;
static MMP_USHORT               m_usOffsetInAtomEntry 	= 0;	///< Offset in the atom. STSC and STTS has 3 or 2 DWs per atom

static MMP_3GP_SAMPLE_INFO      gAudioSample;		///< Audio sample information
static MMP_3GP_SAMPLE_INFO      gVideoSample[2];	///< Video sample information
MMP_3GP_MEDIA_INFO              gAudioMedia;        ///< Audio media information. Most 3GP information stores here.
static MMP_3GP_MEDIA_INFO       gVideoMedia[2]; 	///< Video media information. Most 3GP information stores here.
static MMPF_3GPPSR_PRIVATEDATA  m_3GPPdata;
MMP_VIDEO_CONTAINER_INFO    	gVidFileInfo;       ///< Information of the 3GP file
static MMP_UBYTE                m_ASCInfo[0x30];    ///< The optional AAC information in 3GP
MMP_USERDATA_TAG                m_uDtatag;

static MMP_ULONG                glAVDepackBufAddr;  ///< Address for AV depack buffer
static MMP_ULONG                m_ulCurFileOffset 	= 0; 
static MMPF_3GPPSR_DEPACKCACHE  m_sDepackCache[MAX_DEPACK_CACHE];	///< The 14 AV depack buffers
static MMP_ULONG 				m_ulCacheIdx;  						///< Current cache index. Indicates that which depack buffer is using.
static MMP_ULONG                m_ulBsBufType;
static MMP_BOOL                 m_bBsSupportLargeFrame;
static MMP_UBYTE                *m_VideoStreamBuf;	///< Current video pointer(parser)
static MMP_ULONG                m_ulVideoStreamLen	= 0;
static MMP_ULONG                m_ulAudioPtr		= 0;

static MMP_ULONG                m_ulFirstCTime		= 0;
static MMP_ULONG                m_ulFirstTime		= 0;
static MMP_BOOL                 m_bFastSeek 		= MMP_FALSE;
static MMP_ULONG                m_ulFastSeekBufStart = 0;

MMPF_M_FRAME_INFO            	*m_PrevFrameInfo;
MMPF_M_BS_BUF_INFO 				*m_pBsBufInfo;
#if (EN_SPEED_UP_VID & CACHE_HDL)
MMPF_3GPPSR_ATOM_SEQ_INFO       m_ul3gpAtomParseOrder[MAX_TRACK_NUM][MMPF_3GP_ATOM_MAX] = {0};
MMP_UBYTE                       m_ubAtomOrderByTrk[MAX_TRACK_NUM] = {0};
MMP_BOOL                        m_ubDisMultiCache = MMP_FALSE;
#endif
MMP_BOOL                        m_ubIsThumbNail = MMP_FALSE;
static MMP_ULONG                m_ulDemuxErrorCount = 0;
MMP_ULONG               		m_ulBuffedAudioAtoms;
MMP_ULONG               		m_ulDecodedAudioAtoms;
MMP_ULONG                       m_ulVideoTrackTotalCnt = 0;
MMP_UBYTE                       gubIsVidDualTrks = 0;
#if (MAX_VID_TK == 1)
MMP_ULONG                       m_ulDisVidDualDmux = 1;
#else
MMP_ULONG                       m_ulDisVidDualDmux = 0;
#endif
MMP_UBYTE                       m_ubVideoCurTrack = 0;
MMP_UBYTE                       m_ubVideoDmxTrackNo = 0;
MMP_UBYTE                       m_VideoInfo[2][256];
MMP_UBYTE                       m_VideoInfoLength[2];
MMP_UBYTE                       m_ulBsAlignType;

MMP_FS_FILE     				m_VideoFile[MAX_DEPACK_CACHE];
MMP_FS_FILE_OBJ 				m_VideoFileObj[MAX_DEPACK_CACHE];

#define WRITE_DATA 		 		m_pBsBufInfo->ringData.ptr[RING_WRITE]
#define READ_DATA   			m_pBsBufInfo->ringData.ptr[RING_READ]
#define WRITE_LAP   			m_pBsBufInfo->ringData.lap[RING_WRITE]
#define READ_LAP    			m_pBsBufInfo->ringData.lap[RING_READ]

#define WRITE_HEADER 			(&(m_3GPPdata.curWriteHeader))

//==============================================================================
//
//                              EXTERN VARIABLES
//
//==============================================================================

extern MMP_ULONG 				glAudioPlayBufStart;  		///< The start address of the audio buffer
extern MMP_ULONG 				glAudioPlayBufSize;      	///< The buffer size of the audio buffer (should be 2^N)

extern MMP_ULONG                glAudioPlayReadPtr;     	///< The audio read position in the buffer
extern MMP_ULONG                glAudioPlayWritePtr;     	///< The audio write position in the buffer
extern MMP_ULONG                glAudioPlayReadPtrHigh;		///< read loop counter
extern MMP_ULONG                glAudioPlayWritePtrHigh; 	///< write loop counter

#if (OMA_DRM_EN == 1)
extern MMP_ULONG 				m_glDRMIV;
extern MMP_ULONG 				m_ulDRMIVAddr[MAX_DEPACK_CACHE];
#endif

extern MMP_HANDLE          		*hVideo;
extern MMPF_VIDEO_FUNC     		m_vcodec;
extern MMP_ULONG                m_ulCodecType;

//==============================================================================
//
//                              FUNCTION PROTOTYPE
//
//==============================================================================

static  MMP_ERR     MMPF_VIDPSR_ReadFile(MMP_UBYTE *ptr, const MMP_ULONG SeekPos, const MMP_ULONG size);
static  void 		MMPF_VIDPSR_ReadFromCache(const MMP_ULONG ulSeekPos, MMP_ULONG ulSize, MMP_UBYTE *ubTarget);
static  void 		MMPF_VIDPSR_SwapDepackCacheIdx(const MMP_ULONG ulIndex);
void 				MMPF_VIDPSR_ResetCache(void);

static  MMP_ERR     MMPF_3GPPSR_SeekIFrame(const MMP_SHORT nAdvance, const MMP_ULONG curFrame,MMP_ULONG *RequestFrame, MMP_BOOL bSeekSample);
static  MMP_ERR     MMPF_3GPPSR_GetNextSample(MMP_3GP_MEDIA_INFO *MediaInfo, MMP_3GP_SAMPLE_INFO *pOffsetSizeTime);
static  MMP_ERR     MMPF_3GPPSR_SeekSample(const MMPF_3GP_TRACK track, const MMP_ULONG ulTarget, const MMP_BOOL bByTime);
static  MMP_ULONG   MMPF_3GPPSR_GetTime(const MMP_3GP_MEDIA_INFO *const pMedia);

MMP_ERR             MMPF_Player_GetSupportedFormat(MMP_VID_AUDIOTYPE *supported_audio_format,
                                                   MMP_VID_VIDEOTYPE *supported_video_format);

extern void MMPF_MMU_FlushDCache(void);
extern void MMPF_SetMP3Layer(MMP_UBYTE Layer);
#if (VMP3_P_EN)
extern void MMPF_SetMP3TimeHandle(MMP_ULONG bitrate);
#endif

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

#if 0
void _____Depack_Cache_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_OpenFileForDepackCache
//  Description :
//------------------------------------------------------------------------------
/** @brief Open or close file for depack cache.
This function open and close the file 4 times for each depack buffer.
@param [in] ubFileName The file name. Use NULL for close the depack file
@retval MMP_ERR_NONE Success.
@retval MMP_M_ERR_CONTENT_PIPE_OPEN_FAILED Undefined file error. The error code would be printed out.
@retval MMP_M_ERR_INVALID_STATE Try to close file in memory mode
*/
MMP_ERR MMPF_VIDPSR_OpenFileForDepackCache(MMP_BYTE *ubFileName)
{
    MMP_USHORT	i;
    MMP_ERR		fileStatus;
    
    #if (FS_LIB == FS_UFS)

    if (NULL == ubFileName) {
        fileStatus = MMPF_FS_FClose((MMP_ULONG)m_sDepackCache[0].fileHandle);
    }
    else {
        fileStatus = MMPF_FS_FOpen(gszVidFileName, "rb", (MMP_ULONG*)(&(m_sDepackCache[0].fileHandle)));

        if (fileStatus != MMP_ERR_NONE) {
            return MMP_VIDPSR_ERR_INVALID_STATE;
        }
        
        // Copy file handle and object
        for (i = 1; i < MAX_DEPACK_CACHE; i++) {
            MEMCPY((&m_VideoFile[i]), m_sDepackCache[0].fileHandle, sizeof(MMP_FS_FILE));
            MEMCPY((&m_VideoFileObj[i]), ((MMP_FS_FILE*)m_sDepackCache[0].fileHandle)->pFileObj, sizeof(MMP_FS_FILE_OBJ));
            
            m_VideoFile[i].pFileObj = &m_VideoFileObj[i];
            m_sDepackCache[i].fileHandle = &(m_VideoFile[i]);
        }
    }

    #else
    
	for (i = 0; i < MAX_DEPACK_CACHE; i++) {
	
        if (NULL == ubFileName) {
            fileStatus = MMPF_FS_FClose((MMP_ULONG)m_sDepackCache[i].fileHandle);
            m_sDepackCache[i].fileHandle = 0;
        } 
        else {
    		fileStatus = MMPF_FS_FOpen(gszVidFileName, "rb", (MMP_ULONG *)(&(m_sDepackCache[i].fileHandle)));

            if (MMP_ERR_NONE != fileStatus) {
            	DBG_B(3, i);
            	DBG_B(3, fileStatus);
            	DBG_S(2, " Fail to open the file\r\n");
                return MMP_VIDPSR_ERR_CONTENT_PIPE_OPEN_FAILED;
            }
        }
    }
    
    #endif
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_ReloadCache
//  Description :
//------------------------------------------------------------------------------
/** @brief Reload AV depack cache

This is used to reload the depack cache when the cache is missed.
@param[in] ulSeekPos Position in the 3gp file.
@pre The index is swapped to the cache index
*/
static MMP_ERR MMPF_VIDPSR_ReloadCache(const MMP_ULONG64 ulSeekPos)
{
    MMP_ULONG   ulReadSize = 0;
    MMP_ERR     fileStatus = MMP_ERR_NONE;
    
    // Backward seek or fist-time seek
	MMPF_3GPPSR_DEPACKCACHE *pDepackCache = &(m_sDepackCache[m_ulCacheIdx]);
	
	#if (OMA_DRM_EN == 1)       
	MMP_ULONG64 drmSeekPos = ulSeekPos;
	#endif

	// Backward seek, search from begining
	#if (OMA_DRM_EN == 1)       
    if (gsEnDRM)
    {
        drmSeekPos += DrmFile.DrmInfo.DataStartPos - 16;
        fileStatus = MMPF_FS_FSeek((MMP_ULONG)(pDepackCache->fileHandle), (MMP_LONG64)drmSeekPos, MMP_FS_FILE_BEGIN);
    }
    else
    {
        if (((ulSeekPos - pDepackCache->ulPreSeekPos - DEPACK_CACHE_SIZE) < 0) ||
             (pDepackCache->ulPreSeekPos == 0)) {
            fileStatus = MMPF_FS_FSeek((MMP_ULONG)(pDepackCache->fileHandle), (MMP_LONG64)ulSeekPos, MMP_FS_FILE_BEGIN);
        }
        else {
            fileStatus = MMPF_FS_FSeek((MMP_ULONG)pDepackCache->fileHandle,
                          (MMP_LONG64)ulSeekPos - pDepackCache->ulPreSeekPos - DEPACK_CACHE_SIZE, MMP_FS_FILE_CURRENT);
        }
    }
	
	#else
    
    if ((((MMP_LONG64)ulSeekPos - (MMP_LONG64)pDepackCache->ulPreSeekPos - DEPACK_CACHE_SIZE) < 0) ||
         (pDepackCache->ulPreSeekPos == 0)) {
    	// The file pointer is located at (pDepackCache->ulPreSeekPos + DEPACK_CACHE_SIZE)
        fileStatus = MMPF_FS_FSeek((MMP_ULONG)(pDepackCache->fileHandle), 
        						   (MMP_LONG64)ulSeekPos, MMP_FS_FILE_BEGIN);
    }
    else {
        fileStatus = MMPF_FS_FSeek((MMP_ULONG)pDepackCache->fileHandle,
                      			   (MMP_LONG64)ulSeekPos - pDepackCache->ulPreSeekPos - DEPACK_CACHE_SIZE, MMP_FS_FILE_CURRENT);
    }
	#endif  
  
	#if (OMA_DRM_EN == 1)       
    if (gsEnDRM)
    {
        fileStatus = MMPF_FS_FRead((MMP_ULONG)pDepackCache->fileHandle, (void *)m_glDRMIV, 16, &ulReadSize);	
	    MEMCPY(DrmFile.DrmInfo.ucVEC, (void *)m_glDRMIV, 16);
    }
	#endif

    fileStatus = MMPF_FS_FRead((MMP_ULONG)pDepackCache->fileHandle, pDepackCache->ubAddress,
                               DEPACK_CACHE_SIZE, &ulReadSize);
	
	#if (OMA_DRM_EN == 1)       
    if (gsEnDRM)
    {
        MMPF_DRM_DecryptBufByKey(&DrmFile, (MMP_UBYTE *)(pDepackCache->ubAddress), ulReadSize);	        
    }
	#endif	                               

    // Error and not EOF
    if ((MMP_ERR_NONE != fileStatus) && (MMP_FS_ERR_EOF != fileStatus)) {        
        return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    }
    
	#if (OMA_DRM_EN == 1)       
    if (gsEnDRM)
    {
		pDepackCache->ulPrePagePos = drmSeekPos;
		pDepackCache->ulPreSeekPos = drmSeekPos;
	}
	else
	{
		pDepackCache->ulPrePagePos = ulSeekPos;
		pDepackCache->ulPreSeekPos = ulSeekPos;
	}
	#else
	pDepackCache->ulPrePagePos = ulSeekPos;
	pDepackCache->ulPreSeekPos = ulSeekPos;
	#endif
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_ReadFile
//  Description :
//------------------------------------------------------------------------------
/** @brief Read the 3GP file via depack cache buffers

@param[in] ptr Address to put the read data
@param[in] ulSeekPos 	Offset in the video file.
@param[in] ulSize 		Size to read
@note Currently this function is not thread safe.
@pre The caller should have already align the address with 8 bytes.
@pre The caller should swap the cache index
@return The status of the function call. MMP_ERR_NONE for success.
*/
static MMP_ERR MMPF_VIDPSR_ReadFile(MMP_UBYTE *ptr, const MMP_ULONG ulSeekPos, const MMP_ULONG ulSize)
{
    MMP_ULONG   ulReadedSize, ulCanReadSize, ulRemSize;
    MMP_UBYTE   *ubTarget;
    MMP_ULONG   ulCurSeekPos;
    MMP_ERR     error;
	#if (LARGE_DMA == 1)
	MMPF_3GPPSR_DEPACKCACHE *pDepackCache = &(m_sDepackCache[m_ulCacheIdx]);
	#endif

    ubTarget      	= ptr;
    ulCurSeekPos  	= ulSeekPos;
    ulReadedSize 	= 0;
    ulRemSize    	= ulSize;

    while (ulReadedSize < ulSize) 
    {
        // Compare page number
        if (((pDepackCache->ulPrePagePos) & CACHE_PAGE_MASK) != (ulCurSeekPos & CACHE_PAGE_MASK)) {
            
            // Cache miss, reload the cache and update the old seek position
            #if (VIDEO_NONPREEMPT_FS == 1)
            MMPF_OS_AcquireSem(gFSProtect_SemID, 0);
            #endif
            
            error = MMPF_VIDPSR_ReloadCache(ulCurSeekPos & CACHE_PAGE_MASK);
            
            #if (VIDEO_NONPREEMPT_FS == 1)
            MMPF_OS_ReleaseSem(gFSProtect_SemID);
            #endif
            
            if (error != MMP_ERR_NONE) {
            	return error;
            }	
        }
        
        ulCanReadSize = DEPACK_CACHE_SIZE - (ulCurSeekPos & CACHE_POS_MASK);
		
		// Require next page
        if (ulRemSize > ulCanReadSize) {
        	
        	// Read this page first
            MMPF_VIDPSR_ReadFromCache(ulCurSeekPos, ulCanReadSize, ubTarget);
            
            ulRemSize 		-= ulCanReadSize;
            ulReadedSize 	+= ulCanReadSize;
            ulCurSeekPos 	+= ulCanReadSize;
            ubTarget 		+= ulCanReadSize;
            
			// The caller should have already align the address with 8 bytes.
			#if (LARGE_DMA == 1)
	        if (ulRemSize >= DEPACK_CACHE_SIZE) 
	        { 
	        	// If more than 1 page remains, use large size reading.
				// Continue to read the data in the file position
				
				#if (OMA_DRM_EN == 1)       
		        if (gsEnDRM)
		        {
		        	MEMCPY(DrmFile.DrmInfo.ucVEC, (void *)m_glDRMIV, 16);
		        }
				#endif

                #if (VIDEO_NONPREEMPT_FS == 1)
                MMPF_OS_AcquireSem(gFSProtect_SemID, 0);
                #endif
                
                error = MMPF_FS_FRead((MMP_ULONG)pDepackCache->fileHandle, 
                                      ubTarget, 
                                      (ulRemSize & CACHE_PAGE_MASK), 
                                      &ulCanReadSize);
                
                #if (VIDEO_NONPREEMPT_FS == 1)
                MMPF_OS_ReleaseSem(gFSProtect_SemID);
                #endif

				#if (OMA_DRM_EN == 1)       
		        if (gsEnDRM)
		        {
		            MMPF_DRM_DecryptBufByKey(&DrmFile, ubTarget, ulCanReadSize);	        
		        }
				#endif	                                            
                
                // Will, add check not FS_EOF for play the video with some last frames data lost
	            if ((error != MMP_ERR_NONE) && (error != MMP_FS_ERR_EOF)) {
	        		DBG_S(3, ":Cache Read Error.\r\n");
	        		return error;
	        	}
	        	
	            ulRemSize 		-= ulCanReadSize;
	            ulReadedSize 	+= ulCanReadSize;
	            ulCurSeekPos 	+= ulCanReadSize;
	            ubTarget 		+= ulCanReadSize;
		    	
		    	pDepackCache->ulPrePagePos = ulCurSeekPos - DEPACK_CACHE_SIZE;
    	    	pDepackCache->ulPreSeekPos = ulCurSeekPos - DEPACK_CACHE_SIZE;
	        }
			#endif
        }
        else { 
            // Read remained data
            MMPF_VIDPSR_ReadFromCache(ulCurSeekPos, ulRemSize, ubTarget);
            
            ulReadedSize += ulRemSize;
        }
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_SwapDepackCacheIdx
//  Description :
//------------------------------------------------------------------------------
/** @brief Cache information for video bitstream atom: "mdat", type "vide"

@param[in] index Which cache the video player uses.
*/
static void MMPF_VIDPSR_SwapDepackCacheIdx(const MMP_ULONG ulIndex)
{
	m_ulCacheIdx	= ulIndex;
	#if (OMA_DRM_EN)
	m_glDRMIV 		= m_ulDRMIVAddr[m_ulCacheIdx];
	#endif
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_SetDepackBufAddr
//  Description :
//------------------------------------------------------------------------------
/** @brief Set Depack Buffer

@param[in] ulAddress The starting address of the depack cache
@post Call MMPF_VIDPSR_ResetCache to make this value work
*/
void MMPF_VIDPSR_SetDepackBufAddr(const MMP_ULONG ulAddress)
{
    glAVDepackBufAddr = ulAddress;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_ResetCache
//  Description : Reset the AV depack cache information
//------------------------------------------------------------------------------
void MMPF_VIDPSR_ResetCache(void)
{
    MMP_ULONG i, j;
    
	// Reset the depack cache buffer
	for (i = 0; i < MAX_DEPACK_CACHE; i++) {
		m_sDepackCache[i].ulPrePagePos 	= 0xFFFFFFFF;
		m_sDepackCache[i].ulPreSeekPos 	= 0;
		m_sDepackCache[i].ubAddress   	= (MMP_UBYTE*) (glAVDepackBufAddr + DEPACK_CACHE_SIZE * i);
	}
	
	// Reset the Atom buffer
    if (m_ParserType == MMP_PSR_3GPPARSER) {
    
        for (i = 0; i < MAX_TRACK_NUM; i++) { // 0,1 for Video, 1 for Audio
            
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STSZ] = m_3GPAtomBuf.ulSizeTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STSC] = m_3GPAtomBuf.ulSamplePerChunkTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STCO] = m_3GPAtomBuf.ulChunkOffsetTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STTS] = m_3GPAtomBuf.ulSampleTimeTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STSS] = m_3GPAtomBuf.ulVideoSyncTable;
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_CTTS] = m_3GPAtomBuf.ulCompositionTimeTable;
            
            for (j = 0; j < MMPF_3GP_ATOM_MAX; j++) {
                m_3GPAtomBuf.ulIndex[i][j] = 0x80000000;
            }
        }
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_ReadFromCache
//  Description :
//------------------------------------------------------------------------------
/** @brief Read the data from the depack cache

Abstract Layer for 3gp player to access audio/video/meta data
@param[in] ulSeekPos The offset in the video file.
@param[in] ulSize The size to be read.
@param[out] ubTarget The target address to put the data.
*/
static void MMPF_VIDPSR_ReadFromCache(const MMP_ULONG ulSeekPos, MMP_ULONG ulSize, MMP_UBYTE *ubTarget)
{
    MEMCPY(ubTarget, m_sDepackCache[m_ulCacheIdx].ubAddress + (ulSeekPos & CACHE_POS_MASK), ulSize);
}

#if 0
void _____BitStream_Buffer_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_LockMutex
//  Description : Lock the mutex
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_LockMutex(const MMP_ULONG timeout)
{
    if (MMPF_OS_AcquireSem(m_BSProtect_SemID, timeout) != 0) {
        DBG_S(0, "Acq m_BSProtect_SemID err\r\n");
        return MMP_ERR_NONE;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_UnlockMutex
//  Description : UnLock the mutex
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_UnlockMutex(void)
{
    if (MMPF_OS_ReleaseSem(m_BSProtect_SemID) != 0) {
        DBG_S(0, "Release m_BSProtect_SemID err\r\n");
        return MMP_ERR_NONE;
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_SetBufInfoStrucAddr
//  Description : Set bitstream buffer information structure address.
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_SetBufInfoStrucAddr(MMP_ULONG ulAddr) 
{
    m_pBsBufInfo = (MMPF_M_BS_BUF_INFO*)ulAddr;

    MMPF_BS_ResetBuffer();
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_InitBufInfoStruc
//  Description : Initial bitstream buffer information structure.
//------------------------------------------------------------------------------
void MMPF_BS_InitBufInfoStruc(MMP_ULONG ulVideoInAddr, const MMP_ULONG ulLength, MMP_ULONG ulBufType)
{
    m_pBsBufInfo->buftype	= ulBufType;
    m_pBsBufInfo->length 	= ulLength;
    m_pBsBufInfo->buf    	= (MMP_UBYTE*)ulVideoInAddr;
    m_pBsBufInfo->bufend  	= m_pBsBufInfo->buf + m_pBsBufInfo->length;

    if (m_ulBsBufType == MMP_MEDIA_NEAR_RING_BUFFER) {
        m_bBsSupportLargeFrame = MMP_FALSE;
    }
    else if (m_ulBsBufType == MMP_MEDIA_RING_BUFFER) {
        m_bBsSupportLargeFrame = MMP_TRUE;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_UpdateVideoBufPtr
//  Description : Update video buffer pointer
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_UpdateVideoBufPtr(const RINGBUF_ACCESS_TYPE accessType, const MMP_ULONG ulSize)
{
    MMPF_M_RINGBUF_PTR 	*pRing  	= &m_pBsBufInfo->ringData;
    MMP_ULONG      		ulBufLength = m_pBsBufInfo->length;
    MMP_ULONG       	ulSizeToWrite;
    MMPF_M_FRAME_INFO   *pReadHeader;
    MMP_BOOL            bNeedWrap 	= MMP_FALSE;

    MMPF_BS_LockMutex(VIDPLAY_LOCK_MUTEX_TIME);

    pReadHeader = (MMPF_M_FRAME_INFO*)(m_pBsBufInfo->buf + READ_DATA);

    if (accessType == RING_READ) {
        pRing->Atoms--;
    }

    if (accessType == RING_WRITE) {
    	// For buffer empty case.
        if (pRing->Atoms == 0) {
            if ((pRing->ptr[RING_WRITE] == 0) && 
            	(pRing->lap[RING_WRITE] > pRing->lap[RING_READ])) {
            	
            	pRing->lap[RING_READ]++;
            	pRing->ptr[RING_READ] = 0;
            }
        }
    }

	ulSizeToWrite = ulSize * (accessType == RING_WRITE);
    
	// For buffer empty and write with wrapping
    if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
        if ((pRing->ptr[RING_READ] == pRing->ptr[RING_WRITE]) && 
            (pRing->ptr[RING_WRITE] + ulSizeToWrite > ulBufLength)) {
            
            pRing->lap[RING_READ]++;
            pRing->ptr[RING_READ] = 0;
            pRing->lap[RING_WRITE]++;
            pRing->ptr[RING_WRITE] = 0;
        }
    }
    
    // Update buffer pointer (For Write Access or Read Access + RingBuf)
    if (accessType == RING_WRITE || m_pBsBufInfo->buftype == MMP_MEDIA_RING_BUFFER) 
    {    
        if (ulSize + pRing->ptr[accessType] >= ulBufLength) {

            if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {

                if (ulSize + pRing->ptr[accessType] > ulBufLength) {
                    pRing->ptr[accessType] = ulSize;
                }
                else {
                    pRing->ptr[accessType] = 0;
                }
            }
            else if (m_pBsBufInfo->buftype == MMP_MEDIA_RING_BUFFER) {
           		pRing->ptr[accessType] = pRing->ptr[accessType] + ulSize - ulBufLength;
            }
            
            pRing->lap[accessType]++;
            goto exit_ur; 
        }
        
        pRing->ptr[accessType] += ulSize;
    }
    
exit_ur:
	
	// Update buffer pointer (For Read Access + Near RingBuf)
    if (accessType == RING_READ) {

        if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
            
            bNeedWrap = (pReadHeader->ulFlag & BUFFERFLAG_NEED_WRAP) ? MMP_TRUE : MMP_FALSE;
            
            if (bNeedWrap)
            {
                pRing->lap[RING_READ]++;
                pRing->ptr[RING_READ] = 0;                
            } 
            else {
                pRing->ptr[accessType] += ulSize;
            }
        }
    }
    
    if (accessType == RING_WRITE) {
        pRing->Atoms++;
    }

    MMPF_BS_UnlockMutex();

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_TestVideoBufPtr
//  Description : Check the buffer is overflow/underflow.
//				  The nWrap means the number of buffer pointer need to wraped.
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_TestVideoBufPtr(const RINGBUF_ACCESS_TYPE accessType, const MMP_ULONG ulSize, MMP_USHORT *nWrap)
{
    MMPF_M_RINGBUF_PTR	*pRing  	= &m_pBsBufInfo->ringData;
    MMP_ULONG         	ulBufLength = m_pBsBufInfo->length;
    MMP_ULONG           ulSizeToRead, ulSizeToWrite;
    MMP_ERR             error 		= MMP_ERR_NONE;
    MMP_BOOL            bOverWrite 	= MMP_FALSE;

    MMPF_BS_LockMutex(VIDPLAY_LOCK_MUTEX_TIME);

    ulSizeToRead  = ulSize * (accessType == RING_READ);
    ulSizeToWrite = ulSize * (accessType == RING_WRITE);
    
    *nWrap = 0;

    if (ulSize > ulBufLength) {
        error = MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
        goto exit_tr;
    }

    if ((pRing->ptr[RING_WRITE] + ulSizeToWrite) > (pRing->ptr[RING_READ]) + ulSizeToRead) {
        if (pRing->lap[RING_READ] < (pRing->lap[RING_WRITE])) {
	        // Full
	        error = MMP_VIDPSR_ERR_OVERFLOW;
            goto exit_tr;
        }
    }
    
    if ((pRing->ptr[RING_READ] + ulSizeToRead) > (pRing->ptr[RING_WRITE] + ulSizeToWrite)) {
        if (pRing->lap[RING_READ] == (pRing->lap[RING_WRITE])) {
        	// Underflow
            error = MMP_VIDPSR_ERR_UNDERFLOW;
            goto exit_tr;
        }
    }

    if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
        if ((pRing->ptr[RING_READ] == pRing->ptr[RING_WRITE]) && 
            (pRing->ptr[RING_WRITE] + ulSizeToWrite > ulBufLength)) {
            *nWrap = 2;
            error = MMP_ERR_NONE;
            goto exit_tr;
        }
    }

    if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
        if (pRing->ptr[1 - accessType] < ulSize) {
            bOverWrite = MMP_TRUE;
        }    
    }
    else if (m_pBsBufInfo->buftype == MMP_MEDIA_RING_BUFFER) {
        if ((pRing->ptr[1 - accessType] < (ulSize + pRing->ptr[accessType] - ulBufLength))) {
            bOverWrite = MMP_TRUE;
        }    
    }
    
    if (ulSize + pRing->ptr[accessType] >= ulBufLength) {
        if (!bOverWrite) {
            *nWrap = 1;
      		error = MMP_ERR_NONE;
        }
        else if (accessType == RING_WRITE) {
            *nWrap = 1;
            error = MMP_VIDPSR_ERR_OVERFLOW;
        }
        else {
            error = MMP_VIDPSR_ERR_UNDERFLOW;
        }
    }
    
exit_tr:

    MMPF_BS_UnlockMutex();
    
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_ResetBuffer
//  Description : Reset data pointer
//------------------------------------------------------------------------------
void MMPF_BS_ResetBuffer(void)
{
    MEMSET0(&m_pBsBufInfo->ringData);
    
    glAudioPlayReadPtr    		= 0;
    glAudioPlayWritePtr     	= 0;
    glAudioPlayReadPtrHigh  	= 0;
    glAudioPlayWritePtrHigh 	= 0;
    
    m_ulPreAudioReduntSize    	= 0;
    m_ulBuffedAudioAtoms    	= 0;
    m_ulDecodedAudioAtoms   	= 0;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_ResetAudioState
//  Description :
//------------------------------------------------------------------------------
void MMPF_BS_ResetAudioState(void)
{
    glAudioPlayReadPtr      	= 0;
    glAudioPlayWritePtr     	= 0;
    glAudioPlayReadPtrHigh  	= 0;
    glAudioPlayWritePtrHigh 	= 0;
	
	m_ulPreAudioReduntSize    	= 0;
    m_ulBuffedAudioAtoms    	= 0;
    m_ulDecodedAudioAtoms   	= 0;
    
    m_bAudioEOS           		= MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_AccessVideoHeader
//  Description :
//------------------------------------------------------------------------------
/** @brief Read data from the ring buffer or write the data to the ring buffer

@param[in] pRingBuf The pointer of the ring buffer structure. It should be within the ring buffer boundary.
                    But this function does not check it.
@param[in] pPlainBuf The plain data to be read or write
@param[in] ulSize The size of the data
@param[in] access_type RING_READ to read the data from the ring buffer, and RING_WRITE to write data to the
           ring buffer.
@return Always return success due to no checking the boundary
*/
MMP_ERR MMPF_BS_AccessVideoHeader(MMP_UBYTE* pRingBuf, MMP_UBYTE* pPlainBuf, MMP_ULONG ulSize, 
								  RINGBUF_ACCESS_TYPE access_type)
{
    if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) 
    {
        if (access_type == RING_READ) {
            MEMCPY(pPlainBuf, pRingBuf, ulSize);
        }
        else {
            m_PrevFrameInfo = (MMPF_M_FRAME_INFO*)pRingBuf;
            MEMCPY(pRingBuf, pPlainBuf, ulSize);
        }
    }
    else 
    {
        MMP_LONG lSecondSeg, lFirstSeg;
        MMP_UBYTE* pSrc;
        MMP_UBYTE* pDest;
        
        if (access_type == RING_READ) {
            pSrc = pRingBuf;
            pDest = pPlainBuf;
        }
        else {
            pSrc = pPlainBuf;
            pDest = pRingBuf;
        }

        lSecondSeg = pRingBuf + ulSize - m_pBsBufInfo->bufend;
        lFirstSeg = ulSize;
        
        if (lSecondSeg > 0) {
            lFirstSeg = ulSize - lSecondSeg;
            
            if (access_type == RING_READ) {
                MEMCPY(pDest + lFirstSeg, m_pBsBufInfo->buf, lSecondSeg);
            }
            else {
                MEMCPY(m_pBsBufInfo->buf, pSrc + lFirstSeg, lSecondSeg);
            }
        }

        MEMCPY(pDest, pSrc, lFirstSeg);
    }

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_GetVideoStartAddr
//  Description : Get the address for write video header and frame.
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_GetVideoStartAddr(  MMP_ULONG *pulHeaderAddr, MMP_ULONG *pulFrameAddr,
                                    MMP_ULONG *pulAvailSize,  MMP_ULONG *pulAvailSize1,
                                    MMP_ULONG ulHeaderSize,   MMP_ULONG *ulSize, MMPF_M_BS_ALIGN_TYPE ulAlignType)
{
    MMP_ERR		error;
    MMP_USHORT	nWrap;
    
    if (ulAlignType == BS_ALIGN_256) {
        *ulSize = ALIGN256(*ulSize);
    }           
    error = MMPF_BS_TestVideoBufPtr(RING_WRITE, *ulSize, &nWrap);
    
    if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
        if (nWrap >= 1) {
            m_PrevFrameInfo->ulFlag |= BUFFERFLAG_NEED_WRAP;

            MMPF_BS_LockMutex(VIDPLAY_LOCK_MUTEX_TIME);
            WRITE_DATA = 0;
            WRITE_LAP++;
            MMPF_BS_UnlockMutex();
        }
    }

    if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
    	//EROY CHECK
        if (m_bBsSupportLargeFrame) {
            if (MMPF_BS_TestVideoBufPtr(RING_WRITE, m_pBsBufInfo->length, &nWrap) != MMP_ERR_NONE) {
                return MMP_VIDPSR_ERR_OVERFLOW;
            }

            MMPF_BS_LockMutex(VIDPLAY_LOCK_MUTEX_TIME);
            READ_DATA = WRITE_DATA = 0;
            MMPF_BS_UnlockMutex();
        }
    }

    *pulHeaderAddr = (MMP_ULONG)(m_pBsBufInfo->buf + WRITE_DATA);
    
    *pulFrameAddr = *pulHeaderAddr + ulHeaderSize;
    
    if (m_pBsBufInfo->buftype == MMP_MEDIA_RING_BUFFER)
    {
        if (*pulFrameAddr >= (MMP_ULONG)m_pBsBufInfo->bufend) {
            *pulFrameAddr -= m_pBsBufInfo->length;
        }
    }
    
    // pulAvailSize means the residual space from BS header to buffer end
    // pulAvailSize1 means the residual space from buffer head to read pointer (to avoid overwrite)
    *pulAvailSize  = (MMP_ULONG)(m_pBsBufInfo->bufend) - *pulHeaderAddr;
    *pulAvailSize1 = READ_DATA;
    
    if (m_pBsBufInfo->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {    
        *pulAvailSize1 = 0;
    }
    
    if (ulAlignType == BS_ALIGN_256) {                
        *pulFrameAddr = *pulHeaderAddr + m_3GPPdata.usBSOffset;        
        *pulFrameAddr = ALIGN256(*pulFrameAddr);                  
        //printc(FG_GREEN("\r\n 256AL PSR->time:%x\r\n"), WRITE_HEADER->ulRequestTime);                      
        //printc(AT_DEFAULT("SRC_HEAD:%x\r\n"), *pulHeaderAddr);   
        //printc(AT_DEFAULT("SRC_DATA:%x SIZE:%x(STSZ:%x)\r\n"), *pulFrameAddr, *ulSize, gVideoSample[m_ubVideoDmxTrackNo].ulSize);
    }     
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_GetVideoData
//  Description : Get video frame information.
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_GetVideoData(MMP_ULONG *pulFrameAddr, MMP_ULONG *pulFrameSize, 
							 MMP_ULONG *pulDataSize,  MMP_ULONG *pulFlag, 
							 MMP_ULONG64 *pTime, 	  MMP_ULONG *pulFrameNum, 
							 MMP_UBYTE *pubTrackNo,   MMP_UBYTE *pubIsDualTrack )
{
    MMPF_M_FRAME_INFO header;
    
    if (m_pBsBufInfo->ringData.Atoms == 0) {
        return MMP_VIDPSR_ERR_UNDERFLOW;
    }
        
    MMPF_BS_AccessVideoHeader(m_pBsBufInfo->buf + READ_DATA, (MMP_UBYTE*)(&header), sizeof(MMPF_M_FRAME_INFO), RING_READ);
    
    *pulFrameAddr  	= header.ulAddress;
    *pulFrameSize  	= header.ulBSSize;
    *pulDataSize   	= header.ulSize;
    *pTime       	= header.ulRequestTime;
    *pulFlag       	= header.ulFlag;
    *pulFrameNum 	= header.ulFrameNum;
    *pubTrackNo 	= header.ubTrackNo;
    *pubIsDualTrack = header.ubIsDualTrack;    
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_GetAudioStartAddr
//  Description : Get the address for write audio data.
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_GetAudioStartAddr(MMP_ULONG *pulAddr0, MMP_ULONG *pulAddr1, 
								  MMP_LONG *pulAvailSize, MMP_LONG *pulAvailSize1)
{
    if (glAudioPlayReadPtr == glAudioPlayWritePtr)
    {
        if (glAudioPlayReadPtrHigh == glAudioPlayWritePtrHigh) {
        	// Buffer is empty
            *pulAvailSize = glAudioPlayBufSize;
        }
        else {
            if ((glAudioPlayWritePtrHigh - glAudioPlayReadPtrHigh) != 1)
            {
            	// Error, shouldn't have gone here
                return MMP_VIDPSR_ERR_UNDEFINED;
            }
            else { 
            	// Buffer is full
                *pulAvailSize = 0;
                return MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
            }
        }
    }
    else if (glAudioPlayReadPtr > glAudioPlayWritePtr) {
	    *pulAvailSize = glAudioPlayReadPtr - glAudioPlayWritePtr;
    }
    else {
        *pulAvailSize = glAudioPlayBufSize - (glAudioPlayWritePtr - glAudioPlayReadPtr);
    }
    
    // Note that 0x200 is currently hard coded, half of audio buffer
    if (*pulAvailSize < 0x200) { 
    	// It's too small so that it doesn't load. Wait for reading.
        return MMP_VIDPSR_ERR_OVERFLOW;
    }
   
   	// Multiple of 0x200. if 0x199 < size < 0x400, size = 0x200, 0x400 = 0x400
    *pulAvailSize &= ~(0x200 - 1); 
    
    if (*pulAvailSize == 0) {
        return MMP_VIDPSR_ERR_OVERFLOW;
    }
    
    *pulAddr0 = glAudioPlayBufStart + glAudioPlayWritePtr;

    *pulAvailSize1 = -1;
    
    if ((glAudioPlayBufSize - glAudioPlayWritePtr) > *pulAvailSize) {
    	// Doesn't need to rewind, plain read
    }
    else {
        *pulAddr1    	= glAudioPlayBufStart;
        *pulAvailSize1	= *pulAvailSize - (glAudioPlayBufSize - glAudioPlayWritePtr);
        *pulAvailSize	= glAudioPlayBufSize - glAudioPlayWritePtr;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_UpdateAudioBufptr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_UpdateAudioBufptr(MMP_ULONG ulSize, MMP_BOOL bWrap)
{
    glAudioPlayWritePtr += ulSize;
    
    if (bWrap) {
        glAudioPlayWritePtr     = 0;
        glAudioPlayWritePtrHigh += 1;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_SetAudioPlayReadPtr
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_SetAudioPlayReadPtr(void)
{
#if 0
    MEMCPY(m_pHandShakeBuf+AUDIO_READ_PTR_OFFSET, (MMP_UBYTE*)(&glAudioPlayReadPtr), sizeof(glAudioPlayReadPtr));
    MEMCPY(m_pHandShakeBuf+AUDIO_READ_HIGH_OFFSET, (MMP_UBYTE*)(&glAudioPlayReadPtrHigh), sizeof(glAudioPlayReadPtrHigh));
#endif
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_BS_GetAudioAtomNum
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_BS_GetAudioAtomNum(MMP_ULONG *pulAtomNum)
{
    *pulAtomNum = m_ulBuffedAudioAtoms;
    
    return MMP_ERR_NONE;
}

#if 0
void _____Demuxer_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Ptr2Int
//  Description :
//------------------------------------------------------------------------------
/** @brief Utility to get a unsigned long from specific memory address

Reverse the pointer for big-endian issue.
@param[in] ptr The pointer to be converted
@return The reversed unsigned long value
*/
static MMP_ULONG MMPF_3GPPSR_Ptr2Int(const MMP_UBYTE *ptr)
{
    return (((MMP_ULONG) ptr[0]) << 24) + 
    	   (((MMP_ULONG) ptr[1]) << 16) + 
    	   (((MMP_ULONG) ptr[2]) << 8) + 
           (MMP_ULONG) ptr[3];
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetIntFromAtomBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief Get integer from atom buffer 

@param[in,out] MediaInfo Media Information
@param[in] atomType The atom type.
@param[in] index The index number of requested index.
@return The requested 4 bytes unisnged integer.
*/
static MMP_ERR MMPF_3GPPSR_GetIntFromAtomBuffer(MMP_3GP_MEDIA_INFO 	*MediaInfo, 
												const MMPF_3GP_ATOM atomType, 
          										const MMP_ULONG 	index, 
          										MMP_ULONG 			*value)
{
    static MMP_UBYTE nFields[MMPF_3GP_ATOM_MAX] = {1, 3, 1, 2, 1, 2};
    MMP_UBYTE 	*ptr;
    MMP_ULONG 	track;
    MMP_ULONG 	ulSize;
    MMP_USHORT 	DWperAtom; 		// Number of double words(4 bytes) per atom
    MMP_USHORT 	usRemainder; 	// The offset in the atom buffer
    MMPF_3GP_CACHE_INDEX cache_map;
    //MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
    
#if (MAX_VID_TK == 1)    
    MMP_UBYTE   i;    
    for (i = 0; i < m_ulVideoTrackTotalCnt; i++) {
        if (MediaInfo->ulTrackID == gVideoMedia[i].ulTrackID) {
            track = MMPF_3GP_TRACK_VIDEO;
            cache_map = VIDEO_CHUNK_CACHE + (atomType * MAX_TRACK_NUM);
        }
    }
    
    if (MediaInfo->ulTrackID == gAudioMedia.ulTrackID) {
        track = MMPF_3GP_TRACK_AUDIO;
        cache_map = AUDIO_CHUNK_CACHE + (atomType * MAX_TRACK_NUM);
    }
#else
    if (MediaInfo->ulTrackID == gVideoMedia[0].ulTrackID) {
        track = MMPF_3GP_TRACK_VIDEO_TK0;
        cache_map = VIDEO_CHUNK_CACHE_TK0 + (atomType * MAX_TRACK_NUM);
    }
    else if (MediaInfo->ulTrackID == gVideoMedia[1].ulTrackID) {
        track = MMPF_3GP_TRACK_VIDEO_TK1;
        cache_map = VIDEO_CHUNK_CACHE_TK1 + (atomType * MAX_TRACK_NUM);            
    }    
    else if (MediaInfo->ulTrackID == gAudioMedia.ulTrackID) {
        track = MMPF_3GP_TRACK_AUDIO;
        cache_map = AUDIO_CHUNK_CACHE + (atomType * MAX_TRACK_NUM);
    }
    else {
        printc(FG_RED("NoDef\r\n"));
    }
#endif    
    MMPF_VIDPSR_SwapDepackCacheIdx(cache_map);
    
    DWperAtom = nFields[atomType];
    
    // The constant sample size case.
    if (atomType == MMPF_3GP_ATOM_STSZ) {
        if (MediaInfo->usSampleSize) {
            *value = MediaInfo->usSampleSize;
            return MMP_ERR_NONE;
        }
    }
    
    ulSize = m_3GPAtomBuf.ubSize[atomType];
    usRemainder = index % ulSize;
    
    if ((index < m_3GPAtomBuf.ulIndex[track][atomType]) ||
        ((index - m_3GPAtomBuf.ulIndex[track][atomType]) >= ulSize)) {
        
        // Cache miss, reload the data.
        m_3GPAtomBuf.ulIndex[track][atomType] = index - usRemainder;
        
        if (MMPF_VIDPSR_ReadFile((MMP_UBYTE *)m_3GPAtomBuf.ulTableAddress[track][atomType],
            MediaInfo->ulSTOffset[atomType] + DWperAtom * (((m_3GPAtomBuf.ulIndex[track][atomType]) << 2)),
            DWperAtom * (ulSize << 2)) != MMP_ERR_NONE) {
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    	}
    }
    
    ptr = (MMP_UBYTE *) (&(m_3GPAtomBuf.ulTableAddress[track][atomType][DWperAtom * usRemainder + m_usOffsetInAtomEntry]));

	// Update current accessed sample duration.
    if (atomType == MMPF_3GP_ATOM_STTS) {
        MediaInfo->lDTimeDelta = (MMP_LONG)MMPF_3GPPSR_Ptr2Int(ptr + 4);
    }
    if (atomType == MMPF_3GP_ATOM_CTTS) {
        MediaInfo->lCTimeOffset = (MMP_LONG)MMPF_3GPPSR_Ptr2Int(ptr + 4);
    }
    
    *value = MMPF_3GPPSR_Ptr2Int(ptr);
    
    return MMP_ERR_NONE;
}

#define MMPF_3GPPSR_GetFrameSize(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STSZ, index, value)

#define MMPF_3GPPSR_GetSamplePerChunk(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STSC, index, value)

#define MMPF_3GPPSR_GetFirstChunk(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STSC, index, value)

#define MMPF_3GPPSR_GetChunkOffset(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STCO, index, value)

#define MMPF_3GPPSR_GetSamplePerTime(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STTS, index, value)

#define MMPF_3GPPSR_GetVideoSyncFrame(index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(&gVideoMedia[m_ubVideoCurTrack], MMPF_3GP_ATOM_STSS, index, value)

#define MMPF_3GPPSR_GetSamplePerCTime(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_CTTS, index, value)

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetBigLong
//  Description :
//------------------------------------------------------------------------------
/** @brief Utility to get a unsigned long in the file

Reverse the data for big-endian issue.
@param[in] curSeek The offset in the file
@param[in] advance The bytes to be added to the pointer
@return The reversed unsigned long value
*/
static MMP_ULONG MMPF_3GPPSR_GetBigLong(const MMP_ULONG ulCurSeek, const MMP_SHORT sAdvance, MMP_ULONG *pulValue)
{
    MMP_ERR   error = MMP_ERR_NONE;
    MMP_UBYTE ptr[4];

    error = MMPF_VIDPSR_ReadFile(ptr, ulCurSeek, 4);
    
    if (error != MMP_ERR_NONE) {
        return error;
    }
    
    m_ulCurFileOffset += sAdvance;

    *pulValue = MMPF_3GPPSR_Ptr2Int(ptr);

    return MMP_ERR_NONE;
}

#if 0
void _____Demuxer_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekNextVideoFrame
//  Description :
//------------------------------------------------------------------------------
/** @brief Seek the next video frame

It reads VOP address, seeks the next frame, marks EOS and reports error
@return The error status of the function
@retval MMP_M_ERR_EOS End of video bitstream.
@retval MMP_M_ERR_INSUFFICIENT_RESOURCES When the input bitstream buffer overflow.
@retval MMP_M_ERR_OVERFLOW In ring buffer payload, it has to wait the others to free the buffer
@retval MMP_M_ERR_INCORRECT_STATE_OPERATION The seek mode is not supported.
*/
static MMP_ERR MMPF_3GPPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame)
{
    MMP_ERR error;
	MMP_3GP_MEDIA_INFO 	*pVidMedia;
	MMP_3GP_SAMPLE_INFO *pVidSample;
	MMPF_3GPP_SEEKSTATUS *pSeek;

    if ( MMPF_Player_GetState() == MMP_M_STATE_PAUSE) {
        m_ubVideoDmxTrackNo = m_ubVideoCurTrack;
    }        
    pSeek = &m_seek[m_ubVideoDmxTrackNo];
    pVidMedia = &gVideoMedia[m_ubVideoDmxTrackNo];
    pVidSample = &gVideoSample[m_ubVideoDmxTrackNo];

    *RequestFrame = 0;

    // Get video presentation time offset for following decoded bitstream
    // See PTS/DTS in MPEG for further study.
    if (pSeek->mode == MMPF_M_SEEK_NORMAL) {
        error = MMPF_3GPPSR_GetNextSample(pVidMedia, pVidSample);
    }
    else if (pSeek->mode == MMPF_M_FF_KEY) {
        error = MMPF_3GPPSR_SeekIFrame(pSeek->sSpeed/100, pVidMedia->ulFrameIndex, RequestFrame, MMP_TRUE);
        if (error == MMP_VIDPSR_ERR_EOS) {
            pSeek->mode = MMPF_M_SEEK_NONE;
        }       
    }
    else if (pSeek->mode == MMPF_M_SEEK_NONE) {
        return MMP_VIDPSR_ERR_EOS;
    }
    else {
        return MMP_VIDPSR_ERR_INCORRECT_STATE_OPERATION;
    }

    // calculate the offsets before and after the bitstream in all cases
    m_3GPPdata.usBSOffset = sizeof(MMPF_M_FRAME_INFO);
    if (m_ulCodecType == MMP_VID_VIDEO_MJPG) {
        m_3GPPdata.usBSOffset = ALIGN256(m_3GPPdata.usBSOffset);
    }

    // ARM address padding
    m_3GPPdata.usEOFPadding = (4 - ((pVidSample->ulSize + m_3GPPdata.usBSOffset) % 4)) % 4 + RING_BUF_TAILING;
    
    if ((pVidSample->ulSize + pVidSample->ulOffset) > gVidFileInfo.file_size)
        error = MMP_VIDPSR_ERR_EOS;

    if (m_bBsSupportLargeFrame) {
        // if it's a large frame, use fixed header 0x20;
        if ((pVidSample->ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding) > m_ulVideoStreamLen){
            m_3GPPdata.usBSOffset = 0x20;
        }
    }
	
    return error;
    
    #if (RING_BUF_TAILING % 4) != 0
        #error RING_BUF_TAILING must be a multiple of 4
    #endif
    
    // here we've alreay known the size and offset of the next frame
    // update the frame pointer first to know if right address to read the frame
}  

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekNextSampleTime
//  Description :
//------------------------------------------------------------------------------    
/** @brief Seek the next sample time in the 3GP file

@pre The pSeek->sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of pSeek->sAdvance results in undefined behavior.
@pre The pSeek->ulTargetFrame is set to current sample
@post The pSeek->ulTargetFrame would record which frame the result time belongs to
@param[in] pMedia Which media track to be seeked.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleTime(MMP_3GP_MEDIA_INFO* pMedia) //OK
{
    MMP_ERR error = MMP_ERR_NONE;
    MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];
    
    if (pSeek->sAdvance > 0) 
    {
        if (pMedia->bCttsEnable && pMedia->ulCttsEntryIdx < pMedia->ulCttsEntryCnt) 
        {
            if (pMedia->ulSampleIdxInSttsEntry == (MMP_ULONG)0) {

                do {
                    error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulSttsEntryIdx, &(pMedia->ulSamplesInSttsEntry));
                    
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }    
                    if (pMedia->ulSamplesInSttsEntry == 0) {
                        pMedia->ulSttsEntryIdx += pSeek->sAdvance;
                    }
                } while(pMedia->ulSamplesInSttsEntry == 0);
            }
            
            if (pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame) {
                pMedia->ulSamplesInSttsEntry /= pMedia->ulSamplePerFrame;
                pMedia->lDTimeDelta = pMedia->ulSamplePerFrame;
            }
            
            if (pMedia->ulSampleIdxInCttsEntry == (MMP_ULONG)0) {

                do {
                    error = MMPF_3GPPSR_GetSamplePerCTime(pMedia, pMedia->ulCttsEntryIdx, &(pMedia->ulSamplesInCttsEntry));
                    
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                    if (pMedia->ulSamplesInCttsEntry == 0) {
                        pMedia->ulCttsEntryIdx += pSeek->sAdvance;
                    }
                } while(pMedia->ulSamplesInCttsEntry == 0);
            }
            
            pMedia->ulSampleIdxInSttsEntry += pSeek->sAdvance;
            pMedia->ulSampleIdxInCttsEntry += pSeek->sAdvance;

            // End of stts atom, proceed to stts atom
            if (pMedia->ulSampleIdxInSttsEntry == pMedia->ulSamplesInSttsEntry) {
                pMedia->ulSampleIdxInSttsEntry = (MMP_ULONG)0;
                pMedia->ulSttsEntryIdx += pSeek->sAdvance;
            }
            
            if (pMedia->ulSampleIdxInCttsEntry == pMedia->ulSamplesInCttsEntry) {
                pMedia->ulSampleIdxInCttsEntry = (MMP_ULONG)0;
                pMedia->ulCttsEntryIdx += pSeek->sAdvance;
            }
            
            pMedia->ulDecodingTime += pMedia->lDTimeDelta * pSeek->sAdvance;
            pMedia->ulTickPassTime = (pMedia->ulDecodingTime + pMedia->lCTimeOffset);
        } 
        else
        {
            if (pMedia->ulSampleIdxInSttsEntry == (MMP_ULONG)0) {
            
                do {
                    error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulSttsEntryIdx, &(pMedia->ulSamplesInSttsEntry));
                    
                    if (error != MMP_ERR_NONE) {
                        return error;
					}
                    if(pMedia->ulSamplesInSttsEntry == 0){
                        pMedia->ulSttsEntryIdx += pSeek->sAdvance;
                    }
                } while (pMedia->ulSamplesInSttsEntry == 0);
            }

            if (pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame) {
                pMedia->ulSamplesInSttsEntry /= pMedia->ulSamplePerFrame;
                pMedia->lDTimeDelta = pMedia->ulSamplePerFrame;
            }
            
            pMedia->ulSampleIdxInSttsEntry += pSeek->sAdvance;

            // End of stts atom, proceed to stts atom
            if (pMedia->ulSampleIdxInSttsEntry == pMedia->ulSamplesInSttsEntry) {
                pMedia->ulSampleIdxInSttsEntry = (MMP_ULONG)0;
                pMedia->ulSttsEntryIdx += pSeek->sAdvance;
            }
            
            pMedia->ulTickPassTime += pMedia->lDTimeDelta * pSeek->sAdvance;              
        }
    }
    else 
    {
        if (pMedia->bCttsEnable && pMedia->ulCttsEntryIdx < pMedia->ulCttsEntryCnt)
        {
            pMedia->ulSampleIdxInSttsEntry += pSeek->sAdvance;
            pMedia->ulSampleIdxInCttsEntry += pSeek->sAdvance;
            
            if (pMedia->ulSampleIdxInSttsEntry == (MMP_ULONG)-1) {
        
            	pMedia->ulSttsEntryIdx += pSeek->sAdvance;
            	
            	error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulSttsEntryIdx, &(pMedia->ulSamplesInSttsEntry));
            	
            	if (error != MMP_ERR_NONE) {
            	    return MMP_ERR_NONE;
            	}    
            	if (pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame) {
                    pMedia->ulSamplesInSttsEntry /= pMedia->ulSamplePerFrame;
                    pMedia->lDTimeDelta = pMedia->ulSamplePerFrame;
                }
                
            	pMedia->ulSampleIdxInSttsEntry = pMedia->ulSamplesInSttsEntry - 1;
            }
            
            if (pMedia->ulSampleIdxInCttsEntry == (MMP_ULONG)-1) {
        
                pMedia->ulCttsEntryIdx += pSeek->sAdvance;
                
                error = MMPF_3GPPSR_GetSamplePerCTime(pMedia, pMedia->ulCttsEntryIdx, &(pMedia->ulSamplesInCttsEntry));
                
                if (error != MMP_ERR_NONE) {
                    return MMP_ERR_NONE;
        		}
                
                pMedia->ulSampleIdxInCttsEntry = pMedia->ulSamplesInCttsEntry - 1;
            }
            
            pMedia->ulDecodingTime -= pMedia->lDTimeDelta;
            pMedia->ulTickPassTime = (pMedia->ulDecodingTime + pMedia->lCTimeOffset);
        } 
        else 
        {
            pMedia->ulSampleIdxInSttsEntry += pSeek->sAdvance;
        
            if (pMedia->ulSampleIdxInSttsEntry == (MMP_ULONG)-1) {
            
            	pMedia->ulSttsEntryIdx += pSeek->sAdvance;
        
            	error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulSttsEntryIdx, &(pMedia->ulSamplesInSttsEntry));
            	
            	if (error != MMP_ERR_NONE) {
            	    return MMP_ERR_NONE;
            	}    
            	if (pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame) {
                    pMedia->ulSamplesInSttsEntry /= pMedia->ulSamplePerFrame;
                    pMedia->lDTimeDelta = pMedia->ulSamplePerFrame;
                }
                
            	pMedia->ulSampleIdxInSttsEntry = pMedia->ulSamplesInSttsEntry - 1;
            }
            
            pMedia->ulTickPassTime -= pMedia->lDTimeDelta;
        }
    }

    pSeek->ulTargetFrame += pSeek->sAdvance;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekNextSampleChunk
//  Description :
//------------------------------------------------------------------------------
/** @brief Seek the next sample time in the 3GP file

@pre The pSeek->sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of pSeek->sAdvance results in undefined behavior.
@pre The pSeek->ulTempFrame is set to current sample
@post The pSeek->ulTempFrame would record which frame the result chunk starts from
@param[in] pMedia Which media track to be seeked.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleChunk(MMP_3GP_MEDIA_INFO* pMedia)
{
    MMP_ERR error = MMP_ERR_NONE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
	
    if (pSeek->sAdvance > 0) 
    {
        if (pMedia->ulSampleIdxInChunk == 0) {
            //TL:stsc of multi-entry: Using 0x7FFFFFFF to be last EntryIdx. If not last one, the EntryIdx can still progress until end of entry.
            if (pMedia->lLastChunk != 0x7FFFFFFF) {
				
                m_usOffsetInAtomEntry = 1;
                
                error = MMPF_3GPPSR_GetSamplePerChunk(pMedia, pMedia->ulStscEntryIdx, &(pMedia->ulSamplePerChunk));
                
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                if (pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame) {
                    pMedia->ulSamplePerChunk /= pMedia->ulSamplePerFrame;
                }

                m_usOffsetInAtomEntry = 0;
                
                error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulStscEntryIdx, (MMP_ULONG*)(&pMedia->lFirstChunk));
                
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                // Out of total stsc entries
                if (pMedia->ulStscEntryIdx == pMedia->ulStscEntryCnt - 1) {
                    pMedia->lLastChunk = 0x7FFFFFFF;
                }
                else {
                    error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulStscEntryIdx + 1, (MMP_ULONG*)(&pMedia->lLastChunk));
                    
                    if (error != MMP_ERR_NONE) {
                        return error;
					}
                }

                pMedia->ulStscEntryIdx++; //EROY CHECK: should increase index at first?
            }
            
            pSeek->lChunkToLoad = pMedia->lCurChunk;
            pSeek->ulTempFrame  = pMedia->ulFrameIndex;
        }
        
        pMedia->ulSampleIdxInChunk++;
        
        // End of current chunk, proceed to next chunk
        if (pMedia->ulSampleIdxInChunk == pMedia->ulSamplePerChunk) {
            pMedia->ulSampleIdxInChunk = 0;
            pMedia->lCurChunk++;
        }

        pMedia->ulFrameIndex++;
    }
    else 
    {
        pMedia->ulFrameIndex--;
    	pMedia->ulSampleIdxInChunk--;
    	
        if (pMedia->ulSampleIdxInChunk == (MMP_ULONG) -1) {

            if (pMedia->lCurChunk <= pMedia->lFirstChunk) {
                
                pMedia->ulStscEntryIdx--;

                m_usOffsetInAtomEntry = 1;
                
                error = MMPF_3GPPSR_GetSamplePerChunk(pMedia, pMedia->ulStscEntryIdx, &(pMedia->ulSamplePerChunk));
                
                if (error != MMP_ERR_NONE) {
                    return error;
                }    
                if (pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame) {
                    pMedia->ulSamplePerChunk /= pMedia->ulSamplePerFrame;
                }
                
                m_usOffsetInAtomEntry = 0;
                
                error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulStscEntryIdx, (MMP_ULONG*)(&(pMedia->lFirstChunk)));
                
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulStscEntryIdx + 1, (MMP_ULONG*)(&(pMedia->lLastChunk)));
                
                if (error != MMP_ERR_NONE) {
                    return error;
                }    
            }
            
            pMedia->lCurChunk--;
            
            pSeek->lChunkToLoad = pMedia->lCurChunk;
            pSeek->ulTempFrame  = pMedia->ulFrameIndex + 1; //EROY CHECK
            
            pMedia->ulSampleIdxInChunk = pMedia->ulSamplePerChunk - 1;
        }
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekNextSampleOffset
//  Description :
//------------------------------------------------------------------------------
/** @brief Seek the next sample time in the 3GP file

@pre The pSeek->sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of pSeek->sAdvance results in undefined behavior.
@pre The pSeek->ulTempFrame and ulTargetFrame are assigned or calculated
@param[in] pMedia Which media track to be seeked.
@param[out] pSample Output the offset in the ulOffset field.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleOffset(MMP_3GP_MEDIA_INFO* pMedia, MMP_3GP_SAMPLE_INFO *pSample)
{
    MMP_LONG  i;
    MMP_ULONG value;
    MMP_ERR   error = MMP_ERR_NONE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
    
    if (pSeek->sAdvance > 0) 
    {
        if (pSeek->lChunkToLoad != (MMP_ULONG) -1) {
            error = MMPF_3GPPSR_GetChunkOffset(pMedia, pSeek->lChunkToLoad, &(pSample->ulOffset));
            
            if (error != MMP_ERR_NONE) {
                return error;
            }    
        }
        else {
            pSample->ulOffset = pMedia->ulPreFrameOffset + pMedia->ulPreFrameSize;
        }
        
        for (i = 0; i < pSeek->ulTargetFrame - pSeek->ulTempFrame - 1; i += pSeek->sAdvance) {
        
            error = MMPF_3GPPSR_GetFrameSize(pMedia, pSeek->ulTempFrame + i, &value);
            
            if (error != MMP_ERR_NONE) {
                return error;
            }
            pSample->ulOffset += value * pSeek->sAdvance;
        }
    }
    else 
    {
        if (pSeek->lChunkToLoad != (MMP_ULONG) -1) {
        
            pSeek->ulTempFrame -= pMedia->ulSamplePerChunk; //EROY CHECK
        
            error = MMPF_3GPPSR_GetChunkOffset(pMedia, pSeek->lChunkToLoad, &(pSample->ulOffset));
            
            if (error != MMP_ERR_NONE) {
                return error;
            }    
                
            for (i = 0; i < pSeek->ulTargetFrame - pSeek->ulTempFrame - 1; i++) {
            
                error = MMPF_3GPPSR_GetFrameSize(pMedia, pSeek->ulTempFrame + i, &value);
                
                if (error != MMP_ERR_NONE) {
                    return error;
            	}
           		pSample->ulOffset += value;
            }
        }
        else {
            pSample->ulOffset = pMedia->ulPreFrameOffset;
            
            for (i = 0; i < pSeek->ulTempFrame - pSeek->ulTargetFrame; i ++) {
            
                error = MMPF_3GPPSR_GetFrameSize(pMedia, pSeek->ulTargetFrame + i - 1, &value);
                
                if (error != MMP_ERR_NONE) {
                    return error;
                }    
                pSample->ulOffset -= value;
            }
       }
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekNextSampleSize
//  Description :
//------------------------------------------------------------------------------
/** @brief Seek the next sample time in the 3GP file

@pre The pSeek->sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of pSeek->sAdvance results in undefined behavior.
@pre The pSeek->ulTempFrame and ulTargetFrame are assigned or calculated
@param[in] pMedia Which media track to be seeked.
@param[out] pSample Output the offset in the ulOffset field.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleSize(MMP_3GP_MEDIA_INFO* pMedia, MMP_3GP_SAMPLE_INFO *pSample) //EROY CHECK
{
    MMP_ERR   error  = MMP_ERR_NONE;
    MMP_ULONG ulSize;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];

    error = MMPF_3GPPSR_GetFrameSize(pMedia, pMedia->ulFrameIndex - 1, &ulSize);
    
    if (error != MMP_ERR_NONE) {
        return error;
    }
        
    if (pSeek->sAdvance > 0) {
        pSample->ulSize = ulSize;
        pMedia->ulPreFrameSize = pSample->ulSize;
        pMedia->ulPreFrameOffset = pSample->ulOffset;
    }
    else {
        error = MMPF_3GPPSR_GetFrameSize(pMedia, pMedia->ulFrameIndex, &(pSample->ulSize));
        
        if (error != MMP_ERR_NONE) {
            return error;
		}
		
        pMedia->ulPreFrameSize = ulSize;
        pMedia->ulPreFrameOffset = pSample->ulOffset - pMedia->ulPreFrameSize;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekVideoSyncFrame
//  Description :
//------------------------------------------------------------------------------
/** @brief Search Video frame to sync with audio. Mostly called when FastForward or FastRewind.

This function searches the nearest sync frame (key frame) from current frame.
@param[in] curFrame The current frame. It should be 0~max frame count.
@param[in] sOffset The number of I frame to be advanced. Generally it's been tested by -5~5, 
                   but it's not limit to it. 0 was excluded.
@param[out] pRequestFrame The pointer to out put the index of the request sync frame
@return The status of the function call.
@retval MMP_M_ERR_EOS End of bitstream;
@retval MMP_ERR_NONE Normal search.
@retval MMP_M_ERR_BAD_PARAMETER The request frame is out of range.
*/
static MMP_ERR MMPF_3GPPSR_SeekVideoSyncFrame(const MMP_ULONG ulCurFrame, 
											  const MMP_SHORT sOffset, 
                                              MMP_ULONG *pRequestFrame)
{
    MMP_ULONG 	tmpFrame;
    MMP_LONG 	lTargetIndex;
    MMP_ULONG 	ulMaxSTSSIndex = gVideoMedia[m_ubVideoCurTrack].ulStssEntryCnt - 1;
    MMP_ERR 	error = MMP_ERR_NONE;
    MMP_BOOL 	bEOS = MMP_FALSE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];

    if (ulCurFrame > gVideoMedia[m_ubVideoCurTrack].ulStszEntryCnt) { // gVideoInfo.stszCount = number of video samples/frames
        *pRequestFrame = 0;
        return MMP_VIDPSR_ERR_PARAMETER;
    }
    else 
    {
    	// search the frame. when the number is valid
        // find current index
        lTargetIndex = pSeek->ulFFIndex; // change the calculation into the signed operation.
        
        while(1) {
            error = MMPF_3GPPSR_GetVideoSyncFrame(lTargetIndex,&tmpFrame);
            
            if (error != MMP_ERR_NONE) {
                return error;
            }    
            if (((tmpFrame > ulCurFrame) || (lTargetIndex >= ulMaxSTSSIndex))) {
                break;
            }
            lTargetIndex++;
        };

        if ((tmpFrame > ulCurFrame) &&  (lTargetIndex != 0)){ // backward
            
            lTargetIndex = 0;
            
            do {
                error = MMPF_3GPPSR_GetVideoSyncFrame(lTargetIndex, &tmpFrame);
                if (error != MMP_ERR_NONE)
                    return error;
                
                if(tmpFrame <= ulCurFrame) {
                    lTargetIndex++;
                }
            }
            while(tmpFrame <= ulCurFrame);
            
            if(lTargetIndex != 0)
                lTargetIndex--;
        }

        lTargetIndex += sOffset;

        if ((sOffset < 0) && (lTargetIndex < ulMaxSTSSIndex - 1)) {
            lTargetIndex ++;
        }
        
        if (lTargetIndex <= 0) {
            lTargetIndex = 0;
            error = MMP_VIDPSR_ERR_EOS;
            bEOS = MMP_TRUE;
        }
        else if (lTargetIndex >= ulMaxSTSSIndex) {
            lTargetIndex = ulMaxSTSSIndex;
            error = MMP_VIDPSR_ERR_EOS;
            bEOS = MMP_TRUE;
        }
        pSeek->ulFFIndex = lTargetIndex;
        
        error = MMPF_3GPPSR_GetVideoSyncFrame(pSeek->ulFFIndex,pRequestFrame);
    }
    
    if (bEOS && (error == MMP_ERR_NONE)) {
        error = MMP_VIDPSR_ERR_EOS;
    }
        
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_CheckSeekAtomFromHead
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_3GPPSR_CheckSeekAtomFromHead(MMP_ULONG ulTime)
{
    MMP_3GP_MEDIA_INFO      *MediaInfo = NULL;
    MMP_ULONG               i;
    MMP_3GP_TICK            tickTargetTime;
 
    m_bFastSeek = MMP_FALSE;

    for (i=0; i<MAX_TRACK_NUM; i++) {
    
        if (i == 0)
            MediaInfo =  &gAudioMedia;
        else
            MediaInfo =  &gVideoMedia[i-1];
        
        tickTargetTime = ((MMP_3GP_TICK)ulTime * MediaInfo->ulTimeScale / 1000);
        
        MediaInfo->ulFrameIndex            	= 0;
        MediaInfo->ulTickPassTime        	= 0;
        MediaInfo->ulDecodingTime      		= 0;
        MediaInfo->ulPreFrameSize           = 0;
        MediaInfo->lCurChunk                = 0;
        MediaInfo->lLastChunk               = 0;
        MediaInfo->ulSampleIdxInChunk    	= 0;
        MediaInfo->ulPreFrameOffset         = 0;
        MediaInfo->ulStscEntryIdx           = 0;
        MediaInfo->ulSttsEntryIdx           = 0;
        MediaInfo->ulSampleIdxInSttsEntry	= 0;
        MediaInfo->lDTimeDelta            	= 0;            
        MediaInfo->ulFrameIndex            = 0;
        MediaInfo->ulCttsEntryIdx           = 0;
        MediaInfo->ulSampleIdxInCttsEntry   = 0;
        MediaInfo->lCTimeOffset          	= 0;
    }
               
    m_bFastSeek = MMP_TRUE;

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_FastSeekNextSampleTime
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_3GPPSR_FastSeekNextSampleTime(MMP_ULONG track, MMP_UBYTE* buf, MMP_3GP_MEDIA_INFO* pMedia, MMP_ULONG tickTargetTime, MMP_BOOL bByTime)
{
    MMP_ERR 	error = MMP_ERR_NONE;
    MMP_ULONG	count  = 0, c_count = 0;
	MMP_UBYTE	*ptr, *ptr_c;
	MMP_ULONG	read_size, read_csize;
	MMP_UBYTE	*cache_buf = (MMP_UBYTE*)m_ulFastSeekBufStart;
	MMP_ULONG   ulBufLength = FASTSEEK_BUFFER_SIZE;
	MMP_ULONG	ulSTTSSize = (pMedia->ulSttsEntryCnt * 2 * 4);
	MMP_ULONG	ulCTTSSize = (pMedia->ulCttsEntryCnt * 2 * 4);
    MMP_ULONG   ulGetAtomSuccess = 1;
    MMP_ULONG   ulTargetFrame;
    MMP_ULONG   bNotFinished = MMP_TRUE;
    MMP_BOOL    bSearchCTTS = MMP_FALSE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
    
#if (MAX_VID_TK == 1)
    if (track == MMPF_3GP_TRACK_VIDEO) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));
    }
#else
    if (track == MMPF_3GP_TRACK_VIDEO_TK0) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK0 + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));
    }
    else if (track == MMPF_3GP_TRACK_VIDEO_TK1) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK1 + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));
    }
#endif
    else {// assume that it's audio
	    MMPF_VIDPSR_SwapDepackCacheIdx(AUDIO_CHUNK_CACHE + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));
    }

    if (pMedia->ulSttsEntryCnt == 1 && !(pMedia->bCttsEnable)) {
    
        ptr   = (MMP_UBYTE *)m_3GPAtomBuf.ulTableAddress[track][MMPF_3GP_ATOM_STTS];
    		
		error = MMPF_VIDPSR_ReadFile((MMP_UBYTE *)m_3GPAtomBuf.ulTableAddress[track][MMPF_3GP_ATOM_STTS],
	            					 pMedia->ulSTOffset[MMPF_3GP_ATOM_STTS],
					 		 		 8);
					         		 
		pSeek->ulTargetFrame = pMedia->ulSampleIdxInSttsEntry = (tickTargetTime + pMedia->ulSttsDuration - 1) / pMedia->ulSttsDuration;
		
		if (pSeek->ulTargetFrame == 0) {
		    pSeek->ulTargetFrame         = 
		    pMedia->ulSampleIdxInSttsEntry = 1;
		}
		
		if (pSeek->ulTargetFrame >= (pMedia->ulFrameCount)) {
		    pSeek->ulTargetFrame = pMedia->ulSampleIdxInSttsEntry = pMedia->ulFrameCount;
		}
		
		pMedia->ulTickPassTime = pMedia->ulSampleIdxInSttsEntry * pMedia->ulSttsDuration;
		
		pMedia->ulSamplesInSttsEntry = MMPF_3GPPSR_Ptr2Int(ptr);
    	pMedia->lDTimeDelta 		 = MMPF_3GPPSR_Ptr2Int(ptr + 4);
		pMedia->ulSttsEntryIdx 	= 0;
		
		if((pMedia->ulSamplePerFrame) && (pMedia->ulBytesPerFrame)){
		    pMedia->ulSamplesInSttsEntry /= pMedia->ulSamplePerFrame;
		    pMedia->lDTimeDelta = pMedia->ulSamplePerFrame;
		}
	}
    else
    {  
        MMPF_VIDPSR_ResetCache();
    		
    	read_size  = 64 * 1024;
    	read_csize = 64 * 1024;
    	
    	if (ulBufLength < ((64 * 1024 - 4096)*2)) {
    		read_size = (ulBufLength/2);
    		read_csize = (ulBufLength/2);
    	}
    	
    	if (read_size > ulSTTSSize) {
    		read_size = ulSTTSSize;
    	}
    	
    	if(read_csize > ulCTTSSize){
    	    read_csize = ulCTTSSize;
    	}
    	
    	read_size = ((read_size /4096) + 1) * 4096;
    	
    	read_csize = ((read_csize /4096) + 1) * 4096;
    	
    	read_size = (read_size / 8) * 8;
    	
    	read_csize = (read_csize / 8) * 8;
    	
    	ulTargetFrame           = tickTargetTime;
    	
    	pSeek->ulTargetFrame    = 0;
    	
    	do
    	{
    		if(count == 0) {
#if (MAX_VID_TK == 1)
                if (track == MMPF_3GP_TRACK_VIDEO) {
            	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));
                
                }
#else    		
                if (track == MMPF_3GP_TRACK_VIDEO_TK0) {
            	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK0 + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));                
                }
                else if (track == MMPF_3GP_TRACK_VIDEO_TK1) {
            	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK1 + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));                
                }
#endif
                else {// assume that it's audio
            	    MMPF_VIDPSR_SwapDepackCacheIdx(AUDIO_CHUNK_CACHE + (MMPF_3GP_ATOM_STTS * MAX_TRACK_NUM));
                }
                
    			MMPF_MMU_FlushDCache();
        		
        		count = read_size / 8;
        			
        		ptr   = cache_buf;
        		
        		error = MMPF_VIDPSR_ReadFile(cache_buf,
    						 		 pMedia->ulSTOffset[MMPF_3GP_ATOM_STTS] + 2 * pMedia->ulSttsEntryIdx * 4 ,
    						 		 read_size);
    			if(error != MMP_ERR_NONE)
    	        	return error;
        	}
        	
        	if (c_count == 0 && pMedia->bCttsEnable && pMedia->ulCttsEntryIdx < pMedia->ulCttsEntryCnt){
#if (MAX_VID_TK == 1)
                if (track == MMPF_3GP_TRACK_VIDEO) {
            	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE + (MMPF_3GP_ATOM_CTTS * MAX_TRACK_NUM));
                
                }
#else        	
                if (track == MMPF_3GP_TRACK_VIDEO_TK0) {
            	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK0 + (MMPF_3GP_ATOM_CTTS * MAX_TRACK_NUM));
                }
                else if (track == MMPF_3GP_TRACK_VIDEO_TK1) {
            	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK1 + (MMPF_3GP_ATOM_CTTS * MAX_TRACK_NUM));                
                }
#endif
                else {// assume that it's audio
            	    MMPF_VIDPSR_SwapDepackCacheIdx(AUDIO_CHUNK_CACHE + (MMPF_3GP_ATOM_CTTS * MAX_TRACK_NUM));
                }
        	    
        	    bSearchCTTS = MMP_TRUE;
        	    
    			MMPF_MMU_FlushDCache();
        		
        		c_count = read_csize / 8;
        		
        		ptr_c = (cache_buf+ read_size);
        		
        		error = MMPF_VIDPSR_ReadFile((cache_buf+ read_size),
    						 		 pMedia->ulSTOffset[MMPF_3GP_ATOM_CTTS] + 2 * pMedia->ulCttsEntryIdx * 4 ,
    						 		 read_csize);
    			if(error != MMP_ERR_NONE)
    	        	return error;
        	}
        	
        	ulGetAtomSuccess = 1;
        	
        	if (pMedia->ulSampleIdxInSttsEntry == (MMP_ULONG)0) {
        	    do {
            		pMedia->ulSamplesInSttsEntry = MMPF_3GPPSR_Ptr2Int(ptr);
            		pMedia->lDTimeDelta 		= MMPF_3GPPSR_Ptr2Int(ptr + 4);
            		
            		if((pMedia->ulSamplePerFrame) && (pMedia->ulBytesPerFrame)){
		                pMedia->ulSamplesInSttsEntry /= pMedia->ulSamplePerFrame;
		                pMedia->lDTimeDelta = pMedia->ulSamplePerFrame;
		            }
            		
            		if(pMedia->ulSamplesInSttsEntry == 0){
        	            pMedia->ulSttsEntryIdx += pSeek->sAdvance;
        	        }
        	        
        	        ptr += 2 * 4;
        			count--;
        			
        			if ( (count == 0) && (pMedia->ulSamplesInSttsEntry == 0) )
        			    ulGetAtomSuccess = 0;
        			
        			if(count == 0)
        			    break;
        			    
    			}while (pMedia->ulSamplesInSttsEntry == 0);
        	}
        	
        	if(pMedia->bCttsEnable && pMedia->ulCttsEntryIdx < pMedia->ulCttsEntryCnt){
        	    if (pMedia->ulSampleIdxInCttsEntry == (MMP_ULONG)0) {
            	    do {
                		pMedia->ulSamplesInCttsEntry = MMPF_3GPPSR_Ptr2Int(ptr_c);
                		pMedia->lCTimeOffset 		 = MMPF_3GPPSR_Ptr2Int(ptr_c + 4);
                		
                		if(pMedia->ulSamplesInCttsEntry == 0){
            	            pMedia->ulCttsEntryIdx += pSeek->sAdvance;
            	        }
            	        
            	        ptr_c += 2 * 4;
            			c_count--;
            			
            			if ( (c_count == 0) && (pMedia->ulSamplesInCttsEntry == 0) )
            			    ulGetAtomSuccess = 0;
            			
            			if(c_count == 0)
            			    break;
            			    
        			}while (pMedia->ulSamplesInCttsEntry == 0);
            	}
        	}
        	
        	if (ulGetAtomSuccess) {
        	    if(pMedia->bCttsEnable && pMedia->ulCttsEntryIdx < pMedia->ulCttsEntryCnt){
        	    
        	        pMedia->ulSampleIdxInSttsEntry += pSeek->sAdvance;
        	        pMedia->ulSampleIdxInCttsEntry += pSeek->sAdvance;
        	        
        	        if (pMedia->ulSampleIdxInSttsEntry == pMedia->ulSamplesInSttsEntry) {
            	        pMedia->ulSampleIdxInSttsEntry = (MMP_ULONG)0;
            	        pMedia->ulSttsEntryIdx 		+= pSeek->sAdvance;
            	    }
            	    
            	    if (pMedia->ulSampleIdxInCttsEntry == pMedia->ulSamplesInCttsEntry) {
            	        pMedia->ulSampleIdxInCttsEntry = (MMP_ULONG)0;
            	        pMedia->ulCttsEntryIdx 		+= pSeek->sAdvance;
            	    }
            	    
            	    pMedia->ulDecodingTime += pMedia->lDTimeDelta * pSeek->sAdvance;
            	    pMedia->ulTickPassTime = (pMedia->ulDecodingTime + pMedia->lCTimeOffset);
            	    pSeek->ulTargetFrame 			+= pSeek->sAdvance;
            	    
        	    } else {
                	pMedia->ulSampleIdxInSttsEntry += pSeek->sAdvance;
                
            	    if (pMedia->ulSampleIdxInSttsEntry == pMedia->ulSamplesInSttsEntry) {
            	        pMedia->ulSampleIdxInSttsEntry = (MMP_ULONG)0;
            	        pMedia->ulSttsEntryIdx 		+= pSeek->sAdvance;
            	    }
            	    
            	    pMedia->ulTickPassTime 			+= pMedia->lDTimeDelta * pSeek->sAdvance;
            	    pSeek->ulTargetFrame 			+= pSeek->sAdvance;
        	    }
    	    }
        
    	    if(bByTime) {
    	        MMP_ULONG64 ulltickPassTime = pMedia->ulTickPassTime;
    	        if (pMedia->ulTickPassTime > pMedia->lDTimeDelta)
    	            ulltickPassTime = pMedia->ulTickPassTime - pMedia->lDTimeDelta;
    	        if (ulltickPassTime <= tickTargetTime) {
    	            bNotFinished = MMP_TRUE;
    	        }
    	        else {
    	            bNotFinished = MMP_FALSE;
    	        }
    	        if(pSeek->ulTargetFrame >= (pMedia->ulFrameCount))
    	            bNotFinished = MMP_FALSE;
    	    }
    	    else {
    	        if(pSeek->ulTargetFrame <= ulTargetFrame) {
    	            bNotFinished = MMP_TRUE;
    	        }
    	        else {
    	            bNotFinished = MMP_FALSE;
    	        }
    	    }
    	}
    	while(bNotFinished);
    	
    	MMPF_MMU_FlushDCache();
    		
        MMPF_VIDPSR_ResetCache();
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_FastSeekNextSampleChunk
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_3GPPSR_FastSeekNextSampleChunk(MMP_ULONG track,MMP_UBYTE* buf,MMP_3GP_MEDIA_INFO* pMedia)
{
	MMP_ERR 	error = MMP_ERR_NONE;
	MMP_ULONG	count  = 0;
	MMP_UBYTE	*ptr;
	MMP_ULONG	read_size;
	MMP_UBYTE	*cache_buf = (MMP_UBYTE*)m_ulFastSeekBufStart;
	MMP_ULONG   ulBufLength = FASTSEEK_BUFFER_SIZE;
	MMP_ULONG	ulSTSCSize = (pMedia->ulStscEntryCnt * 3 * 4);
	MMP_ULONG   ulReloadLastChunk = 0;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];	
	
#if (MAX_VID_TK == 1)
    if (track == MMPF_3GP_TRACK_VIDEO) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE + (MMPF_3GP_ATOM_STSC * MAX_TRACK_NUM));
    }
#else	
    if (track == MMPF_3GP_TRACK_VIDEO_TK0) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK0 + (MMPF_3GP_ATOM_STSC * MAX_TRACK_NUM));
    }
    else if (track == MMPF_3GP_TRACK_VIDEO_TK1) {
	    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK1 + (MMPF_3GP_ATOM_STSC * MAX_TRACK_NUM));
    }
#endif    
    else {// assume that it's audio
	    MMPF_VIDPSR_SwapDepackCacheIdx(AUDIO_CHUNK_CACHE + (MMPF_3GP_ATOM_STSC * MAX_TRACK_NUM));
    }   
	
    {
		MMPF_VIDPSR_ResetCache();
		
		read_size = 64 * 1024;
		
		if (ulBufLength < (64 * 1024 - 4096) ) {
			read_size = ulBufLength;
		}
		
		if (read_size > ulSTSCSize) {
			read_size = ulSTSCSize;
		}
		
		read_size = ((read_size /4096) + 1) * 4096;
		
		read_size = (read_size / 12) * 12;
		
		do
	    {
	    	if(count == 0) {
  
	    	    MMPF_MMU_FlushDCache();

	    		count = read_size / 12;
	    		ptr   = cache_buf;

	    		error = MMPF_VIDPSR_ReadFile(cache_buf,
							 		 pMedia->ulSTOffset[MMPF_3GP_ATOM_STSC] + 3 * pMedia->ulStscEntryIdx * 4 ,
							 		 read_size);
				if(error != MMP_ERR_NONE)
		        	return error;
				
				if( ulReloadLastChunk ) {
				    pMedia->lLastChunk = MMPF_3GPPSR_Ptr2Int(ptr);
				    ulReloadLastChunk = 0;
				}
	    	}
	    	
	    	if (pMedia->ulSampleIdxInChunk == 0) {
	    		
	    		if (pMedia->lCurChunk >= pMedia->lLastChunk - 1) {
    	    		pMedia->lFirstChunk 	 = MMPF_3GPPSR_Ptr2Int(ptr);
    	    		
    	    		

    				pMedia->ulSamplePerChunk = MMPF_3GPPSR_Ptr2Int(ptr+4);
    	    		
    	    		if(pMedia->ulSamplePerFrame && pMedia->ulBytesPerFrame){
                        pMedia->ulSamplePerChunk /= pMedia->ulSamplePerFrame;
                    }
    	    		
    	    		count--;
    	    		
    				if (pMedia->ulStscEntryIdx == pMedia->ulStscEntryCnt - 1) {
    			        pMedia->lLastChunk = 0x7FFFFFFF;
    			    }
    			    else {
    			        if(count > 0) {
    			            pMedia->lLastChunk = MMPF_3GPPSR_Ptr2Int(ptr + 12);
    			        }
    			        else {
    			            ulReloadLastChunk = 1;
    			        }
                    }
                
    			    pMedia->ulStscEntryIdx++;
    			    
    			    ptr += 3 * 4;
			    }
		    
		        pSeek->lChunkToLoad = pMedia->lCurChunk;
		        pSeek->ulTempFrame = pMedia->ulFrameIndex;
	    	}
	    	
	    	pMedia->ulSampleIdxInChunk++;
		    // End of current chunk, proceed to next chunk
		    if (pMedia->ulSampleIdxInChunk == pMedia->ulSamplePerChunk) {
		        pMedia->ulSampleIdxInChunk = 0;
		        pMedia->lCurChunk++;
		    }

		    pMedia->ulFrameIndex++;
		        
	    }while((pMedia->ulFrameIndex < pSeek->ulTargetFrame));
		
	    MMPF_MMU_FlushDCache();
		
		MMPF_VIDPSR_ResetCache();
    }
		        
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_FastSeekVideoSyncFrame
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_3GPPSR_FastSeekVideoSyncFrame(const MMP_ULONG curFrame, const MMP_SHORT sOffset, 
                                              MMP_ULONG *pRequestFrame)
{
    MMP_ULONG   tmpFrame;
    MMP_ULONG   CurrFrame,NextFrame = 0;
    MMP_LONG    lTargetIndex;
    MMP_ULONG   ulMaxSTSSIndex = gVideoMedia[m_ubVideoCurTrack].ulStssEntryCnt - 1;
    MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   count = 0;
    MMP_ULONG   read_size;
	MMP_UBYTE	*cache_buf = (MMP_UBYTE*)m_ulFastSeekBufStart;
	MMP_ULONG   ulBufLength = FASTSEEK_BUFFER_SIZE;
    MMP_UBYTE	*ptr;
    MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];
    
    if (curFrame > gVideoMedia[m_ubVideoCurTrack].ulStszEntryCnt) { // gVideoInfo.stszCount = number of video samples/frames
        *pRequestFrame = 0;
        return MMP_VIDPSR_ERR_PARAMETER;
    }
    else {
#if (MAX_VID_TK == 1)
        MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE + (MMPF_3GP_ATOM_STSS * MAX_TRACK_NUM));
#else        
        if (!m_ubVideoDmxTrackNo) {            
            MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK0 + (MMPF_3GP_ATOM_STSS * MAX_TRACK_NUM));
        }
        else {
            MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CHUNK_CACHE_TK1 + (MMPF_3GP_ATOM_STSS * MAX_TRACK_NUM));            
        }
#endif    
        // search the frame. when the number is valid
        // find current index
        lTargetIndex = pSeek->ulFFIndex; // change the calculation into the signed operation.
            
        MMPF_VIDPSR_ResetCache();
    		
    	read_size = 64 * 1024;
    	
    	if (ulBufLength < (64 * 1024 - 4096)) {
    		read_size = ulBufLength;
    	}
    	
    	if (read_size > (gVideoMedia[m_ubVideoCurTrack].ulStssEntryCnt * 4)) {
    		read_size = gVideoMedia[m_ubVideoCurTrack].ulStssEntryCnt * 4;
    	}
    	if(read_size < 512)
    	    read_size = 512;
    	
    	read_size = ((read_size /4096) + 1) * 4096;
    	
    	read_size = (read_size / 8) * 8;
    	
    	pSeek->ulFFIndex = 0;
        
        lTargetIndex = pSeek->ulFFIndex; // change the calculation into the signed operation.
        
        while(1) {
    		if(count == 0) {

    			MMPF_MMU_FlushDCache();

        		count = read_size / 4;
        		ptr   = cache_buf;

        		error = MMPF_VIDPSR_ReadFile(cache_buf,
            						 		 gVideoMedia[m_ubVideoCurTrack].ulSTOffset[MMPF_3GP_ATOM_STSS] + lTargetIndex * 4 ,
            						 		 read_size);

    			if(error != MMP_ERR_NONE)
    	        	return error;
        	}
        	
        	CurrFrame   = NextFrame;
            NextFrame   = MMPF_3GPPSR_Ptr2Int(ptr);
            ptr         += 4;
            count       -= 1;
            
            if (((NextFrame > curFrame) || (lTargetIndex >= ulMaxSTSSIndex))) {
                if(sOffset < 0) {
                    if(lTargetIndex != 0) {
                        tmpFrame = CurrFrame;
                    }
                    else {
                        tmpFrame = NextFrame;
                    }
                }
                else {
                    tmpFrame = NextFrame;
                }
                
                break;
            }
            lTargetIndex++;
            
    	}
        // calculate the requested index
        lTargetIndex += sOffset;

        if ((sOffset < 0) && (lTargetIndex < ulMaxSTSSIndex - 1)) {
            lTargetIndex ++;
        }
        if (lTargetIndex <= 0) {
            lTargetIndex = 0;
            error = MMP_VIDPSR_ERR_EOS;
        }
        else if (lTargetIndex >= ulMaxSTSSIndex) {
            lTargetIndex = ulMaxSTSSIndex;
            error = MMP_VIDPSR_ERR_EOS;
        }
        pSeek->ulFFIndex = lTargetIndex;
        
        *pRequestFrame = tmpFrame;
        
        MMPF_MMU_FlushDCache();
        			
        MMPF_VIDPSR_ResetCache();     
    }
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetNextSample
//  Description :
//------------------------------------------------------------------------------
/** @brief AV sync core for 3gp file.

@param [in,out] pMedia The media info to be serached
@param [out] pSample The search structure of the 3GP file
@return It return the presentation time offset for following decoded bitstream.
*/
static MMP_ERR MMPF_3GPPSR_GetNextSample(MMP_3GP_MEDIA_INFO *pMedia, MMP_3GP_SAMPLE_INFO *pSample)
{
    MMP_ERR error = MMP_ERR_NONE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
#if (EN_SPEED_UP_VID & CACHE_HDL)	   
    MMP_ULONG i, ulAtomCur, ulAtomPrev, ulCurAtomType, ulCacheType;
#endif        
    // Frame index is out of total frame number
    if (pMedia->ulFrameIndex >= pMedia->ulFrameCount) {
        MEMSET0(pSample);
        return MMP_VIDPSR_ERR_EOS;
    }

    // Init the seek
    pSeek->lChunkToLoad = -1;
    pSeek->ulTempFrame 	= pSeek->ulTargetFrame = pMedia->ulFrameIndex;
    pSeek->sAdvance 	= 1;
#if 0//(EN_SPEED_UP_VID & CACHE_HDL)	
	for (i=0; i< 4; i++) {	        
        if (pMedia->ulTrackID == gVideoMedia[0].ulTrackID) {
            ulCurAtomType = m_ul3gpAtomParseOrder[MMPF_3GP_TRACK_VIDEO_TK0][i].ulCurAtom; 
            ulCacheType = VIDEO_CHUNK_CACHE_TK0 + (ulCurAtomType * MAX_TRACK_NUM);
        }
        else if (pMedia->ulTrackID == gVideoMedia[1].ulTrackID) {
            ulCurAtomType = m_ul3gpAtomParseOrder[MMPF_3GP_TRACK_VIDEO_TK1][i].ulCurAtom;
            ulCacheType = VIDEO_CHUNK_CACHE_TK1 + (ulCurAtomType * MAX_TRACK_NUM);            
        }    
        else if (pMedia->ulTrackID == gAudioMedia.ulTrackID) {
            ulCurAtomType = m_ul3gpAtomParseOrder[MMPF_3GP_TRACK_AUDIO][i].ulCurAtom;
            ulCacheType = AUDIO_CHUNK_CACHE + (ulCurAtomType * MAX_TRACK_NUM);   
        }        
      
		if(ulCurAtomType == MMPF_3GP_ATOM_STTS) {		
			if (m_ubDisMultiCache && i) {					
				ulAtomCur = ulCacheType;			
				MMPF_VIDPSR_FileObjCopy(ulAtomCur, ulAtomPrev);					
			}
			ulAtomPrev = ulCacheType;			
			error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);			
		}
		else if (ulCurAtomType == MMPF_3GP_ATOM_STSC) {
			if (m_ubDisMultiCache && i) {						
				ulAtomCur = ulCacheType;
				MMPF_VIDPSR_FileObjCopy(ulAtomCur, ulAtomPrev);
			}		
			ulAtomPrev = ulCacheType;
			error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);			
		}
		else if (ulCurAtomType == MMPF_3GP_ATOM_STCO) {
			if (m_ubDisMultiCache && i) {
				ulAtomCur = ulCacheType;
				MMPF_VIDPSR_FileObjCopy(ulAtomCur, ulAtomPrev);
			}		
			ulAtomPrev = ulCacheType;
			error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample);			
		}
		else if (ulCurAtomType == MMPF_3GP_ATOM_STSZ) {
			if (m_ubDisMultiCache && i) {
				ulAtomCur = ulCacheType;
				MMPF_VIDPSR_FileObjCopy(ulAtomCur, ulAtomPrev);
			}		
			ulAtomPrev = ulCacheType;
			error = MMPF_3GPPSR_SeekNextSampleSize(pMedia, pSample);		
		}
				
	    if (error != MMP_ERR_NONE) {
    	    return error;
	    }		
	}	 	
#else
    error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
    if (error != MMP_ERR_NONE) {
        return error;
    }
        
    error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);
    if(error != MMP_ERR_NONE)
        return error;
        
    error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample);
    if(error != MMP_ERR_NONE)
        return error;
        
    error = MMPF_3GPPSR_SeekNextSampleSize(pMedia, pSample);
    if(error != MMP_ERR_NONE)
        return error;
#endif //#if (EN_SPEED_UP_VID & CACHE_HDL)      
    pSample->lSampleTime = pMedia->lDTimeDelta;

    if (pMedia->ulFrameIndex >= (pMedia->ulFrameCount - 1)) {
        return MMP_VIDPSR_ERR_EOS;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekIFrame
//  Description :
//------------------------------------------------------------------------------
/** @brief Jump to the requested index frame sequntially

@param[in] nAdvance Number of frame to jump. could be negative to backward I frames. Positive for 
                    forward I frames.\n 0 is undefined.
@param[in] curFrame Current frame (INDEX?)
@return The error status of the function call
*/
static MMP_ERR MMPF_3GPPSR_SeekIFrame(const MMP_SHORT nAdvance, const MMP_ULONG curFrame,MMP_ULONG *requestFrame, MMP_BOOL bSeekSample) 
{
	MMP_ERR error = MMP_ERR_NONE;
	MMP_BOOL bEOS = MMP_FALSE;
	MMP_3GP_MEDIA_INFO 	*pVidMedia = &gVideoMedia[m_ubVideoCurTrack];
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];	
    if (nAdvance == 0) {
        return MMP_VIDPSR_ERR_PARAMETER;//undefined
    }
    
    if ( (m_bFastSeek) && (pVidMedia->ulStssEntryCnt != 0) ){
        error = MMPF_3GPPSR_FastSeekVideoSyncFrame(curFrame, nAdvance, requestFrame);
    }
    else {
	    error = MMPF_3GPPSR_SeekVideoSyncFrame(curFrame, nAdvance, requestFrame);
	    if(error == MMP_VIDPSR_ERR_EOS)
	        bEOS = MMP_TRUE;
	}
	
    if ( (error != MMP_VIDPSR_ERR_PARAMETER) && (error != MMP_VIDPSR_ERR_CONTENT_CORRUPT) && (bSeekSample)) {
#if (MAX_VID_TK == 1)
        error = MMPF_3GPPSR_SeekSample(MMPF_3GP_TRACK_VIDEO, *requestFrame, MMP_FALSE);        
#else
        error = MMPF_3GPPSR_SeekSample((!m_ubVideoCurTrack) ? MMPF_3GP_TRACK_VIDEO_TK0 : MMPF_3GP_TRACK_VIDEO_TK1, *requestFrame, MMP_FALSE);        
#endif
	}
	
	pSeek->ulRequestFrame = *requestFrame;
    
    if(error == MMP_ERR_NONE && bEOS)
        error = MMP_VIDPSR_ERR_EOS;
	
	return	error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_SeekSample
//  Description :
//------------------------------------------------------------------------------
/** @brief Make the current atom seek to the media track by time

@param[in] track The video or audio track to be seeked.
@param[in] ulTarget The time to be seeked in ms.
@return The error status of the function call. It always return MMP_ERR_NONE.
*/
static MMP_ERR MMPF_3GPPSR_SeekSample(const MMPF_3GP_TRACK track,
       								  const MMP_ULONG ulTarget, const MMP_BOOL bByTime)
{
	MMP_3GP_TICK tickTargetTime;
	MMP_3GP_MEDIA_INFO *pMedia;
	MMP_3GP_SAMPLE_INFO *pSample;
	MMP_ULONG ulTimeBase;
	MMP_LONG lDifference;
	MMP_ERR  error = MMP_ERR_NONE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];

    if (track == MMPF_3GP_TRACK_AUDIO) {
    	pMedia              = &gAudioMedia;
    	pSample             = &gAudioSample;
        m_ulPreAudioOffset	= 0;
        m_ulPreAudioReduntSize  =0;
    }
#if (MAX_VID_TK == 1)
    else if (track == MMPF_3GP_TRACK_VIDEO) {
#else
    else if (track == MMPF_3GP_TRACK_VIDEO_TK0 || track == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
    	pMedia              = &gVideoMedia[m_ubVideoCurTrack];
    	pSample             = &gVideoSample[m_ubVideoCurTrack];
    }
    else {
        return MMP_VIDPSR_ERR_PARAMETER;
    }
    
    ulTimeBase          = pMedia->ulTimeScale;
	pSeek->sAdvance     = 1;
	pSeek->ulTempFrame  = pSeek->ulTargetFrame = pMedia->ulFrameIndex;

    if (bByTime) {	
        // out of range
    	if (ulTarget > gVidFileInfo.total_file_time) {
    		return MMP_VIDPSR_ERR_PARAMETER;
        }

    	tickTargetTime = ((MMP_3GP_TICK)(ulTarget + pMedia->lDelayTime) * ulTimeBase + 999) / 1000;
    	// find the time to sample
    	if ( (pMedia->ulTickPassTime < tickTargetTime) ||
    	   ((pMedia->ulTickPassTime == 0) && (tickTargetTime == 0)) )
    	{ // forward search
    		if(m_bFastSeek)
    		{
    			error = MMPF_3GPPSR_FastSeekNextSampleTime( track,
        				    						(MMP_UBYTE*)m_3GPAtomBuf.ulTableAddress[track][MMPF_3GP_ATOM_STTS],
        											pMedia,
        											tickTargetTime,
        											MMP_TRUE);
        		
        		if ( error != MMP_ERR_NONE )
                    return error;
    		}
    		else {
	    		while((pMedia->ulTickPassTime < tickTargetTime)) {
	    		    error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
	    		    if (error != MMP_ERR_NONE)
	    		        return error;
	    		}
    		}
    	}
    	else { // backward search
        	pSeek->sAdvance = -1;
    		while((pMedia->ulTickPassTime > tickTargetTime)) {
    		    error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
    		    if ( error != MMP_ERR_NONE ){
    		        DBG_S3("SeekNextSampleTime Error\r\n");
    		        return error;
    		    }
    		    if(pMedia->ulTickPassTime <= (m_ulFirstTime + m_ulFirstCTime))
    		        break;
    		}
            pSeek->ulTargetFrame++;
    	}
    }
    else {
        // out of range
    	if (ulTarget > pMedia->ulFrameCount) {
    		return MMP_VIDPSR_ERR_PARAMETER;
        }
        pSeek->ulTargetFrame = ulTarget;
    }

    lDifference = pSeek->ulTargetFrame - pMedia->ulFrameIndex;

    // find the time to sample
    pSeek->lChunkToLoad = -1; // reset the chunk to load
    pSeek->sAdvance = 1;
    
    if (pMedia->ulFrameIndex < pSeek->ulTargetFrame) { // forward search
    	if(m_bFastSeek)
    	{
    	    
        	error = MMPF_3GPPSR_FastSeekNextSampleChunk(track,
        											(MMP_UBYTE*)m_3GPAtomBuf.ulTableAddress[track][MMPF_3GP_ATOM_STSC],
        											pMedia);
            if ( error != MMP_ERR_NONE )
                return error;
    	}
    	else {
	        while((pMedia->ulFrameIndex < pSeek->ulTargetFrame)) {
	            error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);
	            if ( error != MMP_ERR_NONE )
	                return error;
	        }
        }
        
        error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample);
        if ( error != MMP_ERR_NONE )
        	return error;
    }
    else { // backward search
    	pSeek->sAdvance = -1;
		while((pMedia->ulFrameIndex + 1 > pSeek->ulTargetFrame)) {
            error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);
            if ( error != MMP_ERR_NONE )
                return error;
		}
	    error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample);
	    if ( error != MMP_ERR_NONE )
                return error;
    }
    
    if (pSeek->sAdvance < 0) {
        lDifference --;
    }

    if (!bByTime) { // seek by frame
        for (; 0 != lDifference; lDifference -= pSeek->sAdvance) {
            error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
            if ( error != MMP_ERR_NONE )
                return error;
        }
    }

    error = MMPF_3GPPSR_SeekNextSampleSize(pMedia, pSample);
    if ( error != MMP_ERR_NONE )
        return error;
                
    pSample->lSampleTime = pMedia->lDTimeDelta;

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetSingleFrame
//  Description :
//------------------------------------------------------------------------------
/** @brief Get single frame

Get single VOP from the bitsteam to the bitstream buffer and 
assign the starting position to the HW device.
*/
static MMP_ERR MMPF_3GPPSR_GetSingleFrame(  MMP_ULONG ulDestAddr, MMP_ULONG ulSizeToLoad,
                                            MMP_ULONG ulSeg1    , MMP_ULONG ulSeg2,
                                            MMP_ULONG *pulContainerOffset)
{
    MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   ulRingTailing = 0;
    MMP_UBYTE   *buf_start = m_VideoStreamBuf;
	MMP_3GP_SAMPLE_INFO *pVidSample = &gVideoSample[m_ubVideoDmxTrackNo];
    // Swap cache index to video data
#if (MAX_VID_TK == 1)
    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CACHE);
#else
    MMPF_VIDPSR_SwapDepackCacheIdx( (!m_ubVideoDmxTrackNo) ? VIDEO_CACHE_TK0 : VIDEO_CACHE_TK1);
#endif    
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

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetTime
//  Description :
//------------------------------------------------------------------------------
/** @brief Get the current time in ms from the media info

@param[in] pMedia The media info pointer.
@return The current time in ms.
*/
static MMP_ULONG MMPF_3GPPSR_GetTime(const MMP_3GP_MEDIA_INFO *const pMedia)
{
    MMP_ULONG ulTime;

    if (pMedia->ulTickPassTime < pMedia->lDTimeDelta)
        return 1;

    ulTime = (pMedia->ulTickPassTime - pMedia->lDTimeDelta) * 1000 / pMedia->ulTimeScale;

    if ((MMP_LONG)(ulTime - pMedia->lDelayTime) <= 0)
        ulTime = 1;
    else
        ulTime = ulTime - pMedia->lDelayTime;

    return ulTime;
}

#if 0
void _____Audio_Parser_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_LoadAudioData
//  Description :
//------------------------------------------------------------------------------
/** @brief Load audio data according the the 3GP atoms

Basically it loads one audio frame. But some times if it reaches the limited size, 
it loads partial data and require call twice to load one audio frame.
@param[in] dataptr The address to be loaded
@param[in] Size The size to be loaded
@param[out] LoadedSize The actual loaded size
@return The status of the function call
*/
MMP_ERR MMPF_3GPPSR_LoadAudioData(MMP_UBYTE *dataptr, MMP_ULONG Size, MMP_ULONG *LoadedSize)
{
    typedef struct TEMP_AUDIO_HEADER {
        MMP_ULONG 	ulTime;
        MMP_ULONG 	ulSize;
        MMP_USHORT 	ulFlag;
        MMP_USHORT 	ulFlag2;
    } TEMP_AUDIO_HEADER;
    
    MMP_ULONG           		LoadSize;
    MMP_3GP_SAMPLE_INFO 		tmpAtom;
    MMP_USHORT         	 		usHeaderSizeToWrite = 0;
    MMP_ERR             		error = MMP_ERR_NONE;
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

    if (m_ulPreAudioReduntSize == 0) 
    {       
        error = MMPF_3GPPSR_GetNextSample(&gAudioMedia, &tmpAtom);
        
        curHeader.ulTime = MMPF_3GPPSR_GetTime(&gAudioMedia);
        
        if ((gAudioMedia.usSampleSize <= 4) &&(gAudioMedia.usSampleSize != 0)) 
        {
            MMP_ULONG ulSamples;
        
            if (((gAudioMedia.ulSamplePerChunk - gAudioMedia.ulSampleIdxInChunk) * gAudioMedia.usSampleSize) < 512) {
                ulSamples = (gAudioMedia.ulSamplePerChunk - gAudioMedia.ulSampleIdxInChunk);
                tmpAtom.ulSize += ulSamples * gAudioMedia.usSampleSize;
                gAudioMedia.ulSampleIdxInChunk = 0;
                gAudioMedia.lCurChunk++;    
            } 
            else {
                ulSamples = 512 / gAudioMedia.usSampleSize;
                tmpAtom.ulSize += ulSamples * gAudioMedia.usSampleSize;
                gAudioMedia.ulSampleIdxInChunk += ulSamples;
            }
            
            gAudioMedia.ulFrameIndex += ulSamples;
            gAudioMedia.ulSampleIdxInSttsEntry += ulSamples;
            
            gAudioMedia.ulTickPassTime += ulSamples;
            gAudioMedia.lDTimeDelta += ulSamples;
            curHeader.ulTime = MMPF_3GPPSR_GetTime(&gAudioMedia);
            gAudioMedia.lDTimeDelta -= ulSamples;
            
            gAudioMedia.ulPreFrameSize = tmpAtom.ulSize;
            
            if (gAudioMedia.ulFrameIndex >= (gAudioMedia.ulFrameCount - 1))
                error = MMP_VIDPSR_ERR_EOS;
        }
        
        usHeadherReduntSize = SIZEOF_AUDIO_HEADER;
        m_ulPreAudioReduntSize = tmpAtom.ulSize + SIZEOF_AUDIO_HEADER;
    
        curHeader.ulSize = m_ulPreAudioReduntSize;
        curHeader.ulFlag2 ++;
        curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;
    
        m_ulPreAudioOffset = tmpAtom.ulOffset;
        
        if ((tmpAtom.ulSize + tmpAtom.ulOffset) > gVidFileInfo.file_size)
            error = MMP_VIDPSR_ERR_EOS;
        
        curHeader.ulFlag = BUFFERFLAG_EOF;
        
        if (error == MMP_VIDPSR_ERR_EOS) {
            curHeader.ulFlag |= BUFFERFLAG_EOS;
            m_bAudioEOS = MMP_TRUE;
        }
        m_ulBuffedAudioAtoms++;
    }

    if (m_ulPreAudioReduntSize) 
    {
        // write header
        if (usHeadherReduntSize > 0) { // there is header to write
            
            
            if (usHeadherReduntSize >= Size) {
                usHeaderSizeToWrite = Size;
            }
            else {
                usHeaderSizeToWrite = usHeadherReduntSize;
            }
            
            m_ulPreAudioReduntSize -= usHeaderSizeToWrite;

            memcpy((MMP_UBYTE*)(dataptr), (MMP_UBYTE*)((MMP_UBYTE*)(&curHeader)+(SIZEOF_AUDIO_HEADER - usHeadherReduntSize)), usHeaderSizeToWrite);
            dataptr+=usHeaderSizeToWrite;
           
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

        MMPF_VIDPSR_SwapDepackCacheIdx(AUDIO_CACHE);

        if (MMPF_VIDPSR_ReadFile(dataptr, m_ulPreAudioOffset, LoadSize) != MMP_ERR_NONE) 
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
            
        m_ulPreAudioOffset += LoadSize;

        *LoadedSize = LoadSize + usHeaderSizeToWrite;
    }
    else {
        *LoadedSize = 0;
    }
    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_FillAudioBuffer
//  Description :
//------------------------------------------------------------------------------
/** @brief Fill the audio buffer without being wrapped

@param[in] ulSize The size to be filled in the audio buffer.
@param[in,out] usTransferSize The size to be transfered. It would be accumulated so that it should be
                              initialized well.
@note This function works with audio pointers.
@return Always returns success.
*/
static MMP_ERR MMPF_3GPPSR_FillAudioBuffer(MMP_ULONG ulDestAddr, MMP_ULONG ulSize, MMP_USHORT *usTransferSize)
{
    MMP_ULONG   ulFileReturnSize2 = 1;
    MMP_ERR     error = MMP_ERR_NONE;
    
    *usTransferSize = 0;
    
    while (ulSize > *usTransferSize && ulFileReturnSize2 != 0) {
        
        error = MMPF_3GPPSR_LoadAudioData((MMP_UBYTE *)(ulDestAddr+ *usTransferSize),
                                  		  ulSize - *usTransferSize, &ulFileReturnSize2);
       	
        if (error == MMP_ERR_NONE)
            *usTransferSize += ulFileReturnSize2;
        else
            return error;
    }
    
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Get256SampleTime
//  Description :
//------------------------------------------------------------------------------
/** @brief Look up the samples per frame table of MP4A description

@param[in] sr_index The index of samples per frame
@return The samples per frame
@retval 0 Out of index range
*/
MMP_USHORT MMPF_3GPPSR_Get256SampleTime(const MMP_UBYTE ubSrIndex)
{
    static const MMP_UBYTE sample256_time[] = {
    	0, 0, 0, 21, 23, 32,
    	42, 46, 64, 85, 93, 128
    };
    
    if (ubSrIndex < 12) {
        return sample256_time[ubSrIndex];
    }
    
    return 0;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetSampleRate
//  Description :
//------------------------------------------------------------------------------
/** @brief Look up the sampling rate table of AAC from MP4A description

@param[in] sr_index Index of the table 
@return The sampling rate of of MP4A(AAC)
@retval 0 Out of index range
*/
MMP_ULONG MMPF_3GPPSR_GetSampleRate(const MMP_UBYTE ubSrIndex)
{
    static const MMP_ULONG sample_rates[] = {
        96000, 88200, 64000, 48000, 44100, 32000,
        24000, 22050, 16000, 12000, 11025, 8000
    };

    if (ubSrIndex < 12) {
        return sample_rates[ubSrIndex];
	}
	
    return 0;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_ReadMP4ADescrLength
//  Description :
//------------------------------------------------------------------------------
/** @brief Parse the MP4A description and get its length

@return The length of the description
*/
MMP_ULONG MMPF_3GPPSR_ReadMP4ADescrLength(void)
{
	MMP_UBYTE	b;   
	MMP_UBYTE	numBytes = 0;   
	MMP_ULONG	length = 0;
	MMP_UBYTE	*ptab = (MMP_UBYTE *)(m_ASCInfo);

    do {
    	b = ptab[m_ulAudioPtr++];
        numBytes++;
        length = (length << 7) | (b & 0x7F);
    } while ((b & 0x80) && numBytes < 4);

    return length;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_DecodeMP4ASC
//  Description :
//------------------------------------------------------------------------------
/** @brief Parse the MP4ASC audio header

The atom is located in [moov->trak'soun'->mdia->minf->stbl->stsd->#1.optional description atoms]
@return The parsing result
@retval MMP_M_ERR_UNSUPPORTED_SETTING The format is not supported.
@retval MMP_ERR_NONE Success
*/
MMP_ERR MMPF_3GPPSR_DecodeMP4ASC(MMP_ULONG *ulForamt)
{
	MMP_UBYTE	*ptab = (MMP_UBYTE *)(m_ASCInfo);
	MMP_UBYTE	tmp1, tmp2, ObjectID = 0;
	
	m_ulAudioPtr = 4;
	
	if (ptab[m_ulAudioPtr++] == 0x03) {	// tag
        if (MMPF_3GPPSR_ReadMP4ADescrLength() < 5 + 15) {
            return MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
       	}
		m_ulAudioPtr += 3;	// skip 3 bytes
	}	
	else {
		m_ulAudioPtr += 2;	// skip 2 bytes
	}
		
	if (ptab[m_ulAudioPtr++] != 0x04) {// DescrTab
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}	
			
	if (MMPF_3GPPSR_ReadMP4ADescrLength() < 13) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
		
	ObjectID = ptab[m_ulAudioPtr];

	if (ObjectID == OBJTYPE_MP3) {
	    *ulForamt = MMP_VID_AUDIO_MP3;
	    return MMP_ERR_NONE;
	}
	else if (ObjectID == OBJTYPE_AC3) {
	    *ulForamt = MMP_VID_AUDIO_AC3;
	    return MMP_ERR_NONE;
	}
	else if (ObjectID == OBJTYPE_AAC || ObjectID == OBJTYPE_AAC_M || ObjectID == OBJTYPE_AAC_L) {
	    *ulForamt = MMP_VID_AUDIO_AAC_LC;
	} 
	else {
	    *ulForamt = MMP_VID_AUDIO_NONE;
	    return MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
	
	m_ulAudioPtr += 13;

	if (ptab[m_ulAudioPtr++] != 0x05) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}	

	MMPF_3GPPSR_ReadMP4ADescrLength();
		
	tmp1 = ptab[m_ulAudioPtr++];
	tmp2 = ptab[m_ulAudioPtr++];
		
	mp4ASC.objectTypeIndex 			= (MMP_UBYTE)(tmp1 >> 3);
 	mp4ASC.samplingFrequencyIndex 	= ((tmp1 & 0x07) << 1) | (tmp2 >> 7);
	mp4ASC.channelsConfiguration 	= (tmp2 & 0x78) >> 3;
    mp4ASC.samplingFrequency 		= MMPF_3GPPSR_GetSampleRate(mp4ASC.samplingFrequencyIndex);
	mp4ASC.sample256_time 			= MMPF_3GPPSR_Get256SampleTime(mp4ASC.samplingFrequencyIndex);
	
	if (mp4ASC.samplingFrequency == 0) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
   	}
	
	if (mp4ASC.channelsConfiguration > 7) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
   	}

	if ((mp4ASC.objectTypeIndex == 2) || (mp4ASC.objectTypeIndex == 5) || (mp4ASC.objectTypeIndex == 1)) {
		mp4ASC.frameLengthFlag = (tmp2 & 0x04) >> 2;

		if (mp4ASC.frameLengthFlag == 1) {
	        return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	    }    
	}
	else {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_ParseAudioEsdsAtom
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_3GPPSR_ParseAudioEsdsAtom(MMP_ULONG ulOriginal)
{
    MMP_USHORT usVer = 0;
    MMP_ULONG  ulTmp = 0, Atom = 0;
    MMP_BOOL   bGetESDS = MMP_FALSE;
    MMP_ERR    error = MMP_ERR_NONE;
    
    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 8, 0, &ulTmp);
    if (error != MMP_ERR_NONE) {
        return error;
	}
	
    usVer = ulTmp >> 16;
    
    if (usVer == 0) {
        m_ulCurFileOffset += 28;
    } 
    else if (usVer == 1) {
        m_ulCurFileOffset += (28 + 16);
    } 
    else if (usVer == 2) {
        m_ulCurFileOffset += (28 + 36);
    } 
    else {
        m_ulCurFileOffset = ulOriginal;
        return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
    }
    
    while(!bGetESDS) {

        MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulTmp);
        if (error != MMP_ERR_NONE) {
            m_ulCurFileOffset = ulOriginal;
            return error;
        }

        MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &Atom);
        if (error != MMP_ERR_NONE) {
            m_ulCurFileOffset = ulOriginal;
            return error;
        }

        switch(Atom) {
        case ATOM_WAVE:
      		break;
        case ATOM_ESDS:
            error = MMPF_VIDPSR_ReadFile(m_ASCInfo, m_ulCurFileOffset, 0x30);
            bGetESDS = MMP_TRUE;
            break;
        default:
            m_ulCurFileOffset += (ulTmp - 8);
            break;
        }
    }
    
    m_ulCurFileOffset = ulOriginal;
    
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_GetAudioInfo
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_3GPPSR_GetAudioInfo(MMP_ULONG ulOriginal)
{
    MMP_ERR 	error = MMP_ERR_NONE;
    MMP_ULONG 	ulTmp = 0;
    
	// Parse AudioSampleEntry()
    m_ulCurFileOffset += 16;
    MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulTmp);
    
    gVidFileInfo.audio_info.channel_nums 			= ulTmp >> 16;
    gVidFileInfo.audio_info.bits_per_coded_sample 	= ulTmp & 0xffff;
    
    m_ulCurFileOffset += 4;
    MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulTmp);
    
    gVidFileInfo.audio_info.sampling_rate 			= ulTmp >> 16;
    
    m_ulCurFileOffset = ulOriginal;
    
    return error;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_ParseAudioStsdAtom
//  Description :
//------------------------------------------------------------------------------
static MMP_ERR MMPF_3GPPSR_ParseAudioStsdAtom(MMP_3GP_MEDIA_INFO *pMedia, MMP_ULONG ulOriginal)
{
    MMP_ERR 	error = MMP_ERR_NONE;
    MMP_ULONG 	ulTmp = 0;
    MMP_USHORT 	usVer = 0;
    
    MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 8, 0, &ulTmp);
    usVer = ulTmp >> 16;
    
    m_ulCurFileOffset += 28;
    
    if (usVer == 1) {
        MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 0, &(pMedia->ulSamplePerFrame));
        m_ulCurFileOffset += 8;
        MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 0, &(pMedia->ulBytesPerFrame));
        gVidFileInfo.audio_info.block_align = pMedia->ulBytesPerFrame;
    } 
    else if (usVer == 2) {
        m_ulCurFileOffset += 28;
        MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &(pMedia->ulBytesPerFrame));
        MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 0, &(pMedia->ulSamplePerFrame));
        gVidFileInfo.audio_info.block_align = pMedia->ulBytesPerFrame;
    }
    
    m_ulCurFileOffset = ulOriginal;
    
    return error;
}

/** @} */ // end of name Demuxer

#if 0
void _____3GPPSR_APIS_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Init
//  Description :
//------------------------------------------------------------------------------
/** @brief reset the parser variables

Reset the variable so that the file could be parsed again.
*/
MMP_ERR MMPF_3GPPSR_Init(void)
{
    MMP_ULONG ulTemp;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];    
    
    // Reset sample information
	MEMSET0(&gAudioSample);
	MEMSET0(&gVideoSample[0]);
	MEMSET0(&gVideoSample[1]);
	
	// Reset seek information
	MEMSET0(&m_seek[0]);
	MEMSET0(&m_seek[1]);    
	
	// Reset audio specific config
	MEMSET0(&mp4ASC);
	
	// Reset media information
    MEMSET0(&gAudioMedia);
    MEMSET0(&gVideoMedia[0]);
    MEMSET0(&gVideoMedia[1]);
    
    // Reset 3GP private data
    MEMSET0(&m_3GPPdata);
    
    // Reset 3GP user data
    MEMSET0(&m_uDtatag);
    
    // Reset file information, Keep the file size.
    ulTemp = gVidFileInfo.file_size;
    
    MEMSET0(&gVidFileInfo);
    
    gVidFileInfo.file_size = ulTemp;
	
	// Default set to 1x speed
	pSeek->sSpeed = 10; 
	gDemuxOption  = 0;
	
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Deinit
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_3GPPSR_Deinit(void)
{
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Reset
//  Description :
//------------------------------------------------------------------------------
void MMPF_3GPPSR_Reset(void)
{
    MEMSET0(&m_3GPPdata);
    
    m_bVideoEOS			= MMP_FALSE;
    m_bAudioEOS 		= MMP_FALSE;
    m_ulDemuxErrorCount	= 0;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Demux
//  Description :
//------------------------------------------------------------------------------
/** @brief Demux Audio/Video frame from the bitstream

Triggered by timer2.
Fullfill Audio buffer and Get one Video frame from the bitstream
@return The status of the function call
*/
MMP_ERR MMPF_3GPPSR_Demux(void)
{
    MMP_ERR   	errEOS 	= MMP_ERR_NONE;
    MMP_ERR		error 	= MMP_ERR_NONE;
    MMP_ULONG 	ulBackupRequestFrame, ulRequestFrame = 0;
    MMP_ULONG  	ulSizeToLoad;
    MMP_ULONG 	ulHeaderAddr, ulFrameAddr, ulAvailSize, ulAvailSize1, ulDataSize;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoDmxTrackNo];

    // Demux audio data
	if (gDemuxOption & DEMUX_AUDIO) {
	    
	    // Clear the demux audio option
		gDemuxOption &= ~(DEMUX_AUDIO);
		
        // Fill audio data
        {
            MMP_USHORT  usTransferSize = 0, usTransferSize1 = 0;
            MMP_ULONG   ulAddr, ulAddr1;
            MMP_LONG 	lAvailSize, lAvailSize1;
            MMP_ULONG   ulReturnSize;
            
            ulReturnSize = 0;
             
            if (MMPF_BS_GetAudioStartAddr(&ulAddr, &ulAddr1, &lAvailSize, &lAvailSize1) == MMP_ERR_NONE) {
            
                if (lAvailSize1 < 0) { // Doesn't need to rewind, plain read
                    
                    error = MMPF_3GPPSR_FillAudioBuffer(ulAddr, lAvailSize, &usTransferSize);
                    
                    if ((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS)) {
                        return error;
                    }
                        
                    MMPF_BS_UpdateAudioBufptr(usTransferSize, MMP_FALSE);
                }
                else {// not enough size to put all data, have to rewind
                    error = MMPF_3GPPSR_FillAudioBuffer(ulAddr, lAvailSize, &usTransferSize);
                    
                    if ((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS)) {
                        return error;
                    }
                    
                    if (usTransferSize >= lAvailSize) {
                    
                        MMPF_BS_UpdateAudioBufptr(usTransferSize,MMP_TRUE);

                        error = MMPF_3GPPSR_FillAudioBuffer(ulAddr1, lAvailSize1, &usTransferSize1);
                        
                        if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                            return error;
                        }
                            
                        MMPF_BS_UpdateAudioBufptr(usTransferSize1, MMP_FALSE);
                        usTransferSize += usTransferSize1;
                    } 
                    else {
                        MMPF_BS_UpdateAudioBufptr(usTransferSize, MMP_FALSE);
                    }
                }
            }
        }
	}

	// Demux video data
    if (gDemuxOption & DEMUX_VIDEO) 
    {
    	// Clear the demux video option
 	    gDemuxOption &= ~(DEMUX_VIDEO);
 	    
 	    // Fill video data
        if (!m_3GPPdata.bFillBufferDone) {

            if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame))
            {	
                ulBackupRequestFrame = pSeek->ulRequestFrame;
                
                errEOS = MMPF_3GPPSR_SeekNextVideoFrame(&ulRequestFrame);

                if ((errEOS == MMP_VIDPSR_ERR_EOS) && (m_bVideoEOS)) {
                    return MMP_VIDPSR_ERR_EOS;
                }
                
                pSeek->ulRequestFrame = ulBackupRequestFrame;
		        // ===== Dual Track =====
		        WRITE_HEADER->ubTrackNo     = m_ubVideoDmxTrackNo;
		        WRITE_HEADER->ubIsDualTrack = gubIsVidDualTrks;
                // ===== Time stamp =====
                WRITE_HEADER->ulRequestTime = MMPF_3GPPSR_GetTime(&gVideoMedia[m_ubVideoDmxTrackNo]);
                
                if ((MMP_ERR_NONE == errEOS) || (MMP_VIDPSR_ERR_EOS == errEOS)) {
                    m_3GPPdata.bFilling = MMP_TRUE;
                }
                else {
                    DBG_S(3, " Seek Error\r\n");
                    return errEOS;
                }
            }
            
            if (errEOS == MMP_VIDPSR_ERR_EOS) {
                m_bVideoEOS = MMP_TRUE;
            }
                     
            ulDataSize = gVideoSample[m_ubVideoDmxTrackNo].ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding;
            
            error = MMPF_BS_GetVideoStartAddr(&ulHeaderAddr,
                                              &ulFrameAddr,
                                              &ulAvailSize,
                                              &ulAvailSize1,
                                              m_3GPPdata.usBSOffset,
                                              &ulDataSize,
                                              m_ulBsAlignType);    
            //if (m_ulBsAlignType == BS_ALIGN_256) {            
            //    printc(AT_DEFAULT("SRC_DATA:%x SIZE:%x(STSZ:%x)\r\n"), ulFrameAddr, ulDataSize, gVideoSample[m_ubVideoDmxTrackNo].ulSize);            
            //}
            if ((pSeek->mode == MMPF_M_FF_KEY || pSeek->mode == MMPF_M_SEEK_NONE) &&
               (m_pBsBufInfo->ringData.Atoms > 3)) {
                return MMP_ERR_NONE;
            }
            
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
            
            WRITE_HEADER->ulBSSize      = gVideoSample[m_ubVideoDmxTrackNo].ulSize;
        	WRITE_HEADER->ulAddress     = ulFrameAddr;
        	WRITE_HEADER->ulFlag        = 0;
        	WRITE_HEADER->ulFrameNum    = gVideoMedia[m_ubVideoDmxTrackNo].ulFrameIndex;            

            if (gVideoSample[m_ubVideoDmxTrackNo].ulSize < (ulAvailSize + ulAvailSize1)) {
                
                ulSizeToLoad = gVideoSample[m_ubVideoDmxTrackNo].ulSize;
                
                WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
                
                if (m_bVideoEOS) {
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
        	    }
            }
            else {
                ulSizeToLoad = (ulAvailSize + ulAvailSize1);
            }
            
            gVideoSample[m_ubVideoDmxTrackNo].ulSize -= ulSizeToLoad;
            
            // Copy the write header into the bitstream
            MMPF_BS_AccessVideoHeader((MMP_UBYTE*)ulHeaderAddr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_FRAME_INFO), RING_WRITE);
            
            // Put single VOP for video decoder	            
            error = MMPF_3GPPSR_GetSingleFrame(WRITE_HEADER->ulAddress, ulSizeToLoad, ulAvailSize, ulAvailSize1, &(gVideoSample[m_ubVideoDmxTrackNo].ulOffset));

            if (error != MMP_ERR_NONE) {
                m_3GPPdata.bFilling         = MMP_FALSE;
                m_3GPPdata.bFillBufferDone  = MMP_FALSE;
            
                return error;
            }
            
            m_3GPPdata.bFillBufferDone = MMP_TRUE;
            
#if (DEBUG_VIDEO & LOG_VID_BUF)     
		    if (m_ubVideoCurTrack == m_ubVideoDmxTrackNo) 
		    {
		        printc(FG_GRAY("[3GP]VID_DMX=%x\r\n"), WRITE_HEADER->ulFrameNum + (m_ubVideoDmxTrackNo<<28)); 
		    }
#endif    
            MMPF_BS_UpdateVideoBufPtr(RING_WRITE, WRITE_HEADER->ulSize);
            
            m_3GPPdata.bFilling         = MMP_FALSE;
            m_3GPPdata.bFillBufferDone  = MMP_FALSE;
            m_3GPPdata.pPreWriteHeader.ulRequestTime  = ((MMPF_M_FRAME_INFO*)ulHeaderAddr)->ulRequestTime;
            if (gubIsVidDualTrks) {
                //if (!m_ulDisVidDualDmux)
                {
                    m_ubVideoDmxTrackNo++;                
                    m_ubVideoDmxTrackNo &= 0x01;
                }
            }    
        }
    }

    return  MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_ParseUserData
//  Description :
//------------------------------------------------------------------------------
/** @brief Parse 3GP udta information
@return error state
*/
MMP_ERR MMPF_3GPPSR_ParseUserData(MMP_ULONG ulSize)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_ULONG ulLength = 0;

    m_uDtatag.ubUDtaCount = 0;

    while(ulSize > 0) {

        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, ATOM_LENGTH_SZ, &ulLength);
        if (error != MMP_ERR_NONE) {
            return error;
        }

        ulLength -= 8;
        m_uDtatag.usUDtaLength[m_uDtatag.ubUDtaCount] = ulLength;

        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &m_uDtatag.ulUDtaAtomname[m_uDtatag.ubUDtaCount]);
        if (error != MMP_ERR_NONE) {
            return error;    
        }

        if (ulLength > ATOM_UDTA_MAX_SIZE) {
            // UserData buffer is not enough, just keep the file offset.
            m_uDtatag.bUDtaFileOffset[m_uDtatag.ubUDtaCount] = m_ulCurFileOffset;
            m_uDtatag.bUDta[m_uDtatag.ubUDtaCount][0] = 0;
        }
        else {
            // Save UserData to buffer directly
            m_uDtatag.bUDtaFileOffset[m_uDtatag.ubUDtaCount] = 0;
            error = MMPF_VIDPSR_ReadFile((MMP_UBYTE *)(&m_uDtatag.bUDta[m_uDtatag.ubUDtaCount][0]), m_ulCurFileOffset, ulLength);
            if (error != MMP_ERR_NONE) {
                return error;
            }
        }

        m_ulCurFileOffset += ulLength;
        ulSize -= (ulLength + 8);

        m_uDtatag.ubUDtaCount++;
        if ((m_uDtatag.ubUDtaCount > ATOM_UDTA_MAX_NUM) && ulSize != 0) {
            RTNA_DBG_Str(0, "User data atom number > MAX\r\n");
        }
    }
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_ParseEx
//  Description :
//------------------------------------------------------------------------------
/** @brief Parse 3GP information

@return The validation of the file
*/
MMP_VID_FILEVALIDATION MMPF_3GPPSR_ParseEx(MMP_VIDEO_CONTAINER_INFO* pInfo, MMP_BOOL bInitCodec)
{
	MMP_3GP_MEDIA_INFO      *MediaInfo 		= NULL;
    MMPF_M_MP4V_CONFIG      sMp4vConfig;
    MMP_VID_FILEVALIDATION  fileValidation	= MMP_VID_FILE_VALID;
    MMPF_3GP_TRACK          parsingTrack    = MMPF_3GP_TRACK_UNDEFINED;
    MMP_ULONG               i				= 0;
    MMP_ULONG               ulLength 		= 0;
    MMP_ULONG				ulTmpLength 	= 0;
    MMP_ULONG				ulEntryCnt 		= 0;
    MMP_ULONG               ulAtomName;
    MMP_ULONG               ulTmp, ulValue;
    MMP_ULONG               ulTotalFileSize	= 0;
    MMP_ULONG               ulTrackTimeScale = 0;
    MMP_ULONG               ulTrackDuration = 0;
    MMP_BOOL               	bMdhdFound 		= MMP_FALSE;
    MMP_BOOL                bStsdFound 		= MMP_FALSE;
    MMP_ULONG               ulTrackID 		= 0;
    MMP_UBYTE               *ubDestBuf 		= m_VideoStreamBuf;
    MMP_BOOL                bSupportAudio 	= MMP_TRUE;
    MMP_ERR                 error	 		= MMP_ERR_NONE;
    MMP_BOOL                bPlayAudioOnly 	= MMP_FALSE;
    MMP_ULONG               ulSampleSize 	= 0;
    MMP_USHORT              usWavId 		= 0;
    MMP_ULONG               ulWorkingBuf 	= m_ulWorkingBufStart;
    MMP_LONG                lDelayTime 		= 0;
    MMP_LONG                lDelayRate 		= 0;
    MMP_UBYTE               ubCurVidIndex 	= 0;
    MMP_UBYTE               ubVideoCurTrackTmp = 0;

    // Initial working buffer
    ulWorkingBuf = ALIGN4K(ulWorkingBuf);
	//There are switch nosie when container switch from AVI to 3GP.
	//I will following AVI Buf configuration to avoid this issue
    //ulIDXVBuffer = ulWorkingBuf;
    ulWorkingBuf += FASTSEEK_BUFFER_SIZE;
    //ulIDXABuffer = ulWorkingBuf;
    ulWorkingBuf += FASTSEEK_BUFFER_SIZE;    
    m_ulFastSeekBufStart = ulWorkingBuf;
    ulWorkingBuf += FASTSEEK_BUFFER_SIZE;

    m_ulTempBSSize 		= 0;
    m_ulWorkingBufSize 	= ulWorkingBuf - m_ulWorkingBufStart;
    
    // Reset global parameter
    MEMSET0(&sMp4vConfig);
    
    m_ulFirstCTime 			= 0;
    m_ulFirstTime 			= 0;

    m_ulAudioPtr 			= 0; 
	m_ulPreAudioOffset      = 0;
	m_ulPreAudioReduntSize  = 0;
 
	m_ulCurFileOffset 		= 0;
	m_ulVideoTrackTotalCnt 	= 0; 
#if (EN_SPEED_UP_VID & CACHE_HDL)	
    m_ubAtomOrderByTrk[MMPF_3GP_TRACK_AUDIO] = 0;	
    m_ubAtomOrderByTrk[MMPF_3GP_TRACK_VIDEO_TK0] = 0;
    m_ubAtomOrderByTrk[MMPF_3GP_TRACK_VIDEO_TK1] = 0;
#endif	
	// Reset the parser states
   	MMPF_3GPPSR_Init();
    
    MMPF_Player_GetSupportedFormat(&m_3GPPdata.sSupportedAudioFmt,
                                   &m_3GPPdata.sSupportedVideoFmt);

	ulTotalFileSize = gVidFileInfo.file_size;

	// Reset the parsing cache
    MMPF_VIDPSR_ResetCache();
#if (MAX_VID_TK == 1)
    MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CACHE);
#else    
    MMPF_VIDPSR_SwapDepackCacheIdx( (!m_ubVideoCurTrack) ? VIDEO_CACHE_TK0 : VIDEO_CACHE_TK1);
#endif
	// Parse the file header
	error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, ATOM_LENGTH_SZ, &ulLength);
	if (error != MMP_ERR_NONE) {
	    return error;
	}
	    
	error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 0, &ulAtomName);
	if (error != MMP_ERR_NONE) {
	    return error;    
	}
	
	// File header
    if ((ulAtomName != ATOM_FTYP) && (ulAtomName != ATOM_MOOV) && (ulAtomName != ATOM_MDAT)) {
        
        if ((ulLength + ATOM_LENGTH_SZ + ATOM_TYPE_SZ) < ulTotalFileSize) {
        
            error = MMPF_3GPPSR_GetBigLong(ulLength + ATOM_LENGTH_SZ, 0, &ulAtomName);
            if (error != MMP_ERR_NONE) {
                return error;
            }    
        }
    }

    if (ulAtomName != ATOM_MOOV) {
        m_ulCurFileOffset = ulLength;
    } 
    else {
        m_ulCurFileOffset += ATOM_TYPE_SZ;
    }
   	
	// Parse the 3GP file body
    while (m_ulCurFileOffset < ulTotalFileSize) 
    {
        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, ATOM_LENGTH_SZ, &ulLength);
        if (error != MMP_ERR_NONE) {
            return error;
        }
        
        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, ATOM_TYPE_SZ, &ulAtomName);
        if (error != MMP_ERR_NONE) { 
            return error;
        }
               
        if (ulLength == 0 && ulAtomName != ATOM_MOOV) {
            break;
        }
		//Avoid m_ulCurFileOffset overflow on 4G files
        if (ulLength > (0xFFFFFFFF - m_ulCurFileOffset)) {
            DBG_S3("3GPPSR_ParseEx handle 4G file Overflow\r\n");
            break;
        }

        switch (ulAtomName) {
        //"ftyp" QuickTime Prefs file types [P:NULL, C:NULL]
        case ATOM_FTYP:
        	DBG_S3("3GPPSR_ParseEx ATOM_FTYP\r\n");
            m_ulCurFileOffset += (ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ);
            break;
        //"mdat" Media Data Atom [P:NULL, C:Chunk]
        case ATOM_MDAT:
        	DBG_S3("3GPPSR_ParseEx ATOM_MDAT\r\n");
            m_ulCurFileOffset += (ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ);
            break; 
        //"iods" Initial Object Description Atom
        case ATOM_IODS:
        	DBG_S3("3GPPSR_ParseEx ATOM_IODS\r\n");
            m_ulCurFileOffset += (ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ);
            break;
        //"vmhd" Video Media Header Atom. [P:minf, C:NULL]
        case ATOM_VMHD:
        	DBG_S3("3GPPSR_ParseEx ATOM_VMHD\r\n");
            m_ulCurFileOffset += (ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ);
            break;
        //"smhd" Sound Media Header Atom. [P:minf, C:NULL]
        case ATOM_SMHD:
        	DBG_S3("3GPPSR_ParseEx ATOM_SMHD\r\n");
            m_ulCurFileOffset += (ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ);
            break;
        //"dinf" Data Information Atom. [P:minf, C:dref]
        case ATOM_DINF:
        	DBG_S3("3GPPSR_ParseEx ATOM_DINF\r\n");
            m_ulCurFileOffset += (ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ);
            break;
        //"mvhd" Movie Header Atom [P:moov, C:NULL]
        case ATOM_MVHD:
        	DBG_S3("3GPPSR_ParseEx ATOM_MVHD\r\n");
        	
            // Get Time scale
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ + 8, 0, &(gVidFileInfo.total_file_time));
            if (error != MMP_ERR_NONE) {
                return error;
			}
	
            // Get Duration    
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ + 12, ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ, &ulValue);
            if (error != MMP_ERR_NONE) {
                return error;
            }
                
            gVidFileInfo.total_file_time = ((MMP_3GP_TICK)ulValue) * 1000 / gVidFileInfo.total_file_time;
            break;
        //"moov" Movie Atom [P:NULL, C:mvhd, trak]
        case ATOM_MOOV:
        	DBG_S3("3GPPSR_ParseEx ATOM_MOOV\r\n");
        	break;
        //"mdia" Media Atom [P:trak, C:mdhd, hdlr, minf]    
        case ATOM_MDIA:
        	DBG_S3("3GPPSR_ParseEx ATOM_MDIA\r\n");
            break;
        //"trak" Track Atom. [P:moov, C:tkhd, mdia]
        case ATOM_TRAK:
            DBG_S3("3GPPSR_ParseEx ATOM_TRAK\r\n");
            bMdhdFound      = MMP_FALSE;
            bStsdFound      = MMP_FALSE;
            lDelayTime      = 0;
            lDelayRate      = 0;
            parsingTrack    = MMPF_3GP_TRACK_UNDEFINED;
            break;
        //"tkhd" Track Header Atom. [P:trak, C:NULL]
        case ATOM_TKHD:
            DBG_S3("3GPPSR_ParseEx ATOM_TKHD\r\n");
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ + 8, ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ, &ulTrackID);
            if (error != MMP_ERR_NONE) {
                return error;
            }    
            break;
        //"edts" Edit Atom. [P:trak, C:elst] 
        case ATOM_EDTS:
        	DBG_S3("3GPPSR_ParseEx ATOM_EDTS\r\n");
            break;
        //"elst" Edit List Atom [P:edts, C:elst]    
        case ATOM_ELST:
            DBG_S3("3GPPSR_ParseEx ATOM_ELST\r\n");
            
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ + 8, 16, (MMP_ULONG *)&lDelayTime);
            if (error != MMP_ERR_NONE) {
                return error;
            }
                
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, (MMP_ULONG *)&lDelayRate);
            if (error != MMP_ERR_NONE) {
                return error;
            }
            
            if (lDelayTime == (-1)) {
                lDelayTime = 0;
                lDelayRate = 0;
            }
            
            if (parsingTrack != MMPF_3GP_TRACK_UNDEFINED) {
                MediaInfo->lMediaTime = lDelayTime;
                MediaInfo->lMediaRate = lDelayRate;
            }
            m_ulCurFileOffset += ulLength - 28;
            break;
        //"minf" Media Information Atom. [P:mdia C:vmhd, smhd, dinf, stbl]
        case ATOM_MINF:
            DBG_S3("3GPPSR_ParseEx ATOM_MINF\r\n");
            
            if (parsingTrack == MMPF_3GP_TRACK_UNDEFINED) {
            
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ulLength + 8, 0, &ulTmp); //CHECK!

                if (error != MMP_ERR_NONE) {
                    return error;
                }
                    
                switch (ulTmp) {
                case ATOM_SOUN:
                    //"soun"
                    gVidFileInfo.is_audio_available  		= MMP_TRUE;
                    parsingTrack                            = MMPF_3GP_TRACK_AUDIO;
                    MediaInfo                               = &gAudioMedia;
                    MediaInfo->ulTrackID                    = ulTrackID;
                    MediaInfo->lMediaTime                   = lDelayTime;
                    MediaInfo->lMediaRate                   = lDelayRate;
                    break;
                case ATOM_VIDE:
                    //"vide"
                    if (m_ulVideoTrackTotalCnt) {
                        ubCurVidIndex++; 
                    }
                    
                    gVidFileInfo.is_video_available[ubCurVidIndex] = MMP_TRUE;
#if (MAX_VID_TK == 1)
                    parsingTrack                            = MMPF_3GP_TRACK_VIDEO;
#else
                    parsingTrack                            = (!ubCurVidIndex)? MMPF_3GP_TRACK_VIDEO_TK0 : MMPF_3GP_TRACK_VIDEO_TK1;
#endif
                    MediaInfo                               = &gVideoMedia[ubCurVidIndex];
                    MediaInfo->ulTrackID                    = ulTrackID;
                    MediaInfo->lMediaTime                   = lDelayTime;
                    MediaInfo->lMediaRate                   = lDelayRate;
               
                    m_ulVideoTrackTotalCnt++;
#if (MAX_VID_TK == 1)                    
                    gubIsVidDualTrks = MMP_FALSE;
#else
                    gubIsVidDualTrks = (m_ulVideoTrackTotalCnt<2) ? MMP_FALSE : MMP_TRUE;
#endif
                    break;
                default:
                    parsingTrack                            = MMPF_3GP_TRACK_UNDEFINED;
                    break;
                }
            }
            break;    
        //"mdhd" Media Header Atom. [P:mdia C:NULL]
        case ATOM_MDHD:
            DBG_S3("3GPPSR_ParseEx ATOM_MDHD\r\n");
            
            bMdhdFound = MMP_TRUE;
            
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ + 8, 16, &ulTrackTimeScale);
            if (error != MMP_ERR_NONE) {
                return error;
            }
            
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, ulLength - ATOM_LENGTH_SZ - ATOM_TYPE_SZ - 16, &ulTrackDuration);    
            if (error != MMP_ERR_NONE) {
                return error;
			}
			
            if (bStsdFound) {
                MediaInfo->ulTimeScale = ulTrackTimeScale;
                
                if (parsingTrack == MMPF_3GP_TRACK_AUDIO) {
                    gVidFileInfo.audio_info.total_audio_time = ulTrackDuration;
                }
#if (MAX_VID_TK == 1)
                else if (parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
                else if (parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
                         parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
                    gVidFileInfo.video_info[ubCurVidIndex].total_video_time = ulTrackDuration;
                }
            }
            break;
        //"hdlr" Media Handler Reference Atom. [P:mdia C:NULL]
        case ATOM_HDLR:
            DBG_S3("3GPPSR_ParseEx ATOM_HDLR\r\n");
            
            // Parsing Audio or Video Format
            error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 8, 4 + 8, &ulTmp);
            if (error != MMP_ERR_NONE) {
                return error;
            }
                
            if (parsingTrack == MMPF_3GP_TRACK_UNDEFINED) {    
                switch (ulTmp) {
                case ATOM_SOUN:
                    //"soun" Sound header
                    gVidFileInfo.is_audio_available  		= MMP_TRUE;
                    parsingTrack                            = MMPF_3GP_TRACK_AUDIO;
                    MediaInfo                               = &gAudioMedia;
                    MediaInfo->ulTrackID                    = ulTrackID;
                    MediaInfo->lMediaTime                   = lDelayTime;
                    MediaInfo->lMediaRate                   = lDelayRate;
#if (EN_SPEED_UP_VID & CACHE_HDL)				
                    m_ubAtomOrderByTrk[parsingTrack]          = 0;
#endif					
                    break;
                case ATOM_VIDE:
                    //"vide" Video header
                    if (m_ulVideoTrackTotalCnt) {
                        ubCurVidIndex++; 
                    }
                    
                    gVidFileInfo.is_video_available[ubCurVidIndex] = MMP_TRUE;
#if (MAX_VID_TK == 1)
                    parsingTrack                            = MMPF_3GP_TRACK_VIDEO;
#else
                    parsingTrack                            = (!ubCurVidIndex)? MMPF_3GP_TRACK_VIDEO_TK0 : MMPF_3GP_TRACK_VIDEO_TK1;                    
#endif
                    MediaInfo                               = &gVideoMedia[ubCurVidIndex];
                    MediaInfo->ulTrackID                    = ulTrackID;
                    MediaInfo->lMediaTime                   = lDelayTime;
                    MediaInfo->lMediaRate                   = lDelayRate;
#if (EN_SPEED_UP_VID & CACHE_HDL)						
                    m_ubAtomOrderByTrk[parsingTrack]          = 0;
#endif					
                    m_ulVideoTrackTotalCnt++;
#if (MAX_VID_TK == 1)                    
                    gubIsVidDualTrks = MMP_FALSE;
#else                    
                    gubIsVidDualTrks = (m_ulVideoTrackTotalCnt<2) ? MMP_FALSE : MMP_TRUE;                    
#endif
                    break;
                default:
                    parsingTrack                            = MMPF_3GP_TRACK_UNDEFINED;
                    break;
                }
            }
            m_ulCurFileOffset += ulLength - 12 - 8;
            break;          
        //"stbl" Sample Table Atom [P:minf C:stsd, stts, stsc, stsz, stco, stss]
        case ATOM_STBL:
            DBG_S3("3GPPSR_ParseEx ATOM_STBL\r\n");
            break; 
        //"stsd" Sample Description Atom [P:stbl, C:avc1, ms]
        case ATOM_STSD:
            DBG_S3("3GPPSR_ParseEx ATOM_STSD\r\n");
            
            bStsdFound = MMP_TRUE;
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO) 
            {
                if (bMdhdFound) {
                    MediaInfo->ulTimeScale = ulTrackTimeScale;
                    gVidFileInfo.audio_info.total_audio_time = ulTrackDuration;
                }
                
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 12, 4 + 12, &ulTmp);
                if (error != MMP_ERR_NONE) {
                    return error;
				}
				
                #if (VWAV_P_EN)
                MMPF_SetWAVFormat(MMP_FALSE);
                #endif
                
                switch (ulTmp) {
                case CODEC_MP4A:
                    bSupportAudio = MMP_TRUE;
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_AAC_LC;
                    break;
                case CODEC_AMR:
                    bSupportAudio = MMP_TRUE;
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_AMR_NB;
                    break;
                case CODEC_PCM_1:
                    bSupportAudio = MMP_TRUE;
                    MMPF_3GPPSR_GetAudioInfo(m_ulCurFileOffset);
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_WAV;
                    ulSampleSize = 2 * gVidFileInfo.audio_info.channel_nums;
                    break;
                case CODEC_PCM_2:
                    bSupportAudio = MMP_TRUE;
                    MMPF_3GPPSR_GetAudioInfo(m_ulCurFileOffset);
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_WAV;
                    ulSampleSize = 2 * gVidFileInfo.audio_info.channel_nums;
                    break;
                case CODEC_PCM_ALAW:
                    bSupportAudio = MMP_TRUE;
                    MMPF_3GPPSR_GetAudioInfo(m_ulCurFileOffset);
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_WAV;
                    ulSampleSize = 1 * gVidFileInfo.audio_info.channel_nums;
                    usWavId = 0x06;
                    break;
                case CODEC_PCM_ULAW:
                    bSupportAudio = MMP_TRUE;
                    MMPF_3GPPSR_GetAudioInfo(m_ulCurFileOffset);
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_WAV;
                    ulSampleSize = 1 * gVidFileInfo.audio_info.channel_nums;
                    usWavId = 0x07;
                    break;
                case CODEC_MP3_1:
                    bSupportAudio = MMP_TRUE; 
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_MP3;
                    break;
                case CODEC_MP3_2:
                    bSupportAudio = MMP_TRUE; 
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_MP3;
                    break;
                case CODEC_AC3:
                    bSupportAudio = MMP_TRUE; 
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_AC3;
                    break;
                case CODEC_PCM_IMA_WAV:
                    bSupportAudio = MMP_TRUE;
                    MMPF_3GPPSR_GetAudioInfo(m_ulCurFileOffset);
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_WAV;
                    gVidFileInfo.audio_info.bits_per_coded_sample = 4;
                    usWavId = 0x11;
                    break;
                case CODEC_PCM_IMA_QT:
                    bSupportAudio = MMP_TRUE;
                    MMPF_3GPPSR_GetAudioInfo(m_ulCurFileOffset);
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_WAV;
                    usWavId = 0x12;
                    break;
                default :
                    bSupportAudio = MMP_FALSE;
                    gVidFileInfo.audio_info.format = MMP_VID_AUDIO_UNSUPPORTED;
                    gVidFileInfo.is_audio_available = MMP_FALSE;
                    break;
                }

                if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AAC_LC) {
                    
                    MMP_ULONG ulFormat = MMP_VID_AUDIO_NONE;    
                    
                    error = MMPF_3GPPSR_ParseAudioEsdsAtom(m_ulCurFileOffset);
                    if (error != MMP_ERR_NONE) {
                        return error;
					}
					
					error = MMPF_3GPPSR_DecodeMP4ASC(&ulFormat);
					
					gVidFileInfo.audio_info.format = ulFormat;
					
                    if (error != MMP_ERR_NONE) {
                        DBG_S3("MP4ASC decode error!!!!\r\n");
                        bSupportAudio = MMP_FALSE;
                        gVidFileInfo.audio_info.format = MMP_VID_AUDIO_NONE;
                        gVidFileInfo.is_audio_available = MMP_FALSE;
                    }

                    m_ulCurFileOffset += ulLength - 24;
                }
                else if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_MP3) {
                    MMPF_3GPPSR_ParseAudioStsdAtom(MediaInfo, m_ulCurFileOffset);
                    m_ulCurFileOffset += ulLength - 24;
                }
                else if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_WAV) {
                    MMPF_3GPPSR_ParseAudioStsdAtom(MediaInfo, m_ulCurFileOffset);
                    m_ulCurFileOffset += ulLength - 24;
                }
                else {
                    m_ulCurFileOffset += ulLength - 24;
                }
            }
#if (MAX_VID_TK == 1)
            else if (parsingTrack == MMPF_3GP_TRACK_VIDEO) 
#else
            else if (parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 || parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) 
#endif
            {
                MMP_ULONG ulSkipLen = 0;
                
                if (bMdhdFound) {
                    MediaInfo->ulTimeScale = ulTrackTimeScale;
                    gVidFileInfo.video_info[ubCurVidIndex].total_video_time = ulTrackDuration;
                }
                
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 12, 4 + 12, &ulTmp);
                if (error != MMP_ERR_NONE) {
                    return error;
				}
				
                if (ulTmp == CODEC_H263 || ulTmp == CODEC_S263) {

                    gVidFileInfo.video_info[ubCurVidIndex].format = MMP_VID_VIDEO_H263;
                    sMp4vConfig.usHWConfig |= VLD_H263_SHORT_HDR;
                    
                    if ((m_3GPPdata.sSupportedVideoFmt & gVidFileInfo.video_info[ubCurVidIndex].format) == 0)
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    } 
                }
                else if (ulTmp == CODEC_MP4V) {

                    gVidFileInfo.video_info[ubCurVidIndex].format = MMP_VID_VIDEO_MP4V_SP;
                    
                    if ((m_3GPPdata.sSupportedVideoFmt & gVidFileInfo.video_info[ubCurVidIndex].format) == 0)
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    } 
                }
                else if (ulTmp == CODEC_H264) {

                    gVidFileInfo.video_info[ubCurVidIndex].format = MMP_VID_VIDEO_H264;
                    
                    if ((m_3GPPdata.sSupportedVideoFmt & gVidFileInfo.video_info[ubCurVidIndex].format) == 0)
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                }
                else if (ulTmp == CODEC_MJPA) {

                    gVidFileInfo.video_info[ubCurVidIndex].format = MMP_VID_VIDEO_MJPG;
                    
                    if ((m_3GPPdata.sSupportedVideoFmt & gVidFileInfo.video_info[ubCurVidIndex].format) == 0)
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                }
                else {
                    gVidFileInfo.video_info[ubCurVidIndex].format = MMP_VID_VIDEO_UNSUPPORTED;
                    
                    return MMP_VID_UNSUPPORTED_VIDEO;
                }
                
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 24, 4 + 24, &ulTmp);
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                gVidFileInfo.video_info[ubCurVidIndex].width  = (MMP_USHORT)(ulTmp >> 16);
                gVidFileInfo.video_info[ubCurVidIndex].height = (MMP_USHORT) ulTmp;

				// Parse optional description atom 'esds'
                if (bInitCodec && gVidFileInfo.video_info[ubCurVidIndex].format == MMP_VID_VIDEO_MP4V_SP) 
                {
                    MMP_ULONG ulDesc = 0;
                    
                    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 50, 4 + 50, &ulTmpLength);
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                    
                    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulDesc);
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                        
                    while (ulDesc != ATOM_ESDS) {
                        m_ulCurFileOffset += (ulTmpLength - 8);
                        ulSkipLen += ulTmpLength;
                        
                        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulTmpLength);
                        if (error != MMP_ERR_NONE) {
                            return error;
                        }
                            
                        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulDesc);
                        if (error != MMP_ERR_NONE) {
                            return error;
                    	}
                    }
                    
                    error = MMPF_VIDPSR_ReadFile(ubDestBuf, m_ulCurFileOffset, ulTmpLength);
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                    
                    gVidFileInfo.video_info[ubCurVidIndex].width  = 0;
                    gVidFileInfo.video_info[ubCurVidIndex].height = 0;
                    
                    {                     
                        hVideo = MMPF_VIDEO_Open(CODEC_MP4V, &m_vcodec);

                        if (MMP_ERR_NONE != 
                            m_vcodec.MMPF_VIDDEC_Init(	hVideo,
                            							ubDestBuf, 
                            							ulTmpLength,
                                                   		gVidFileInfo.video_info[ubCurVidIndex].width,
                                                   		gVidFileInfo.video_info[ubCurVidIndex].height,
                                                   		MMP_TRUE)) {
                                                  
                            DBG_S(3,"Init video failed\r\n");
                            bPlayAudioOnly = MMP_TRUE;
                        }
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo, &gVidFileInfo.video_info[ubCurVidIndex]);
                    }
                    
                    m_ulCurFileOffset += ulLength - 102 - 8 - ulSkipLen;
                }
                else if (bInitCodec && gVidFileInfo.video_info[ubCurVidIndex].format == MMP_VID_VIDEO_H264) 
                {
                    MMP_ULONG ulDesc = 0;
                    
                    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 50, 4 + 50, &ulTmpLength);
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                    
                    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulDesc);
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                        
                    while (ulDesc != ATOM_AVCC) {
                        m_ulCurFileOffset += (ulTmpLength - 8);
                        ulSkipLen += ulTmpLength;
                        
                        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulTmpLength);
                        if (error != MMP_ERR_NONE) {
                            return error;
                        }
                            
                        error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &ulDesc);
                        if (error != MMP_ERR_NONE) {
                            return error;
                    	}
                    }   
                    
                    error = MMPF_VIDPSR_ReadFile(ubDestBuf, m_ulCurFileOffset, ulTmpLength);
                    if (error != MMP_ERR_NONE) {
                        return error;
					}

                    {                        
                        hVideo = MMPF_VIDEO_Open(CODEC_H264, &m_vcodec);
                        
                        if (ulTmpLength > 256) {
                            RTNA_DBG_Str(0, "ERROR!!, Can't backup Video info(SPS/PPS)\r\n");
                        }
                        
                        MEMCPY(&m_VideoInfo[ubCurVidIndex][0], (MMP_UBYTE *)ubDestBuf, ulTmpLength);
                        m_VideoInfoLength[ubCurVidIndex] = ulTmpLength;
                        
                        m_vcodec.MMPF_VIDDEC_ConnectBSBuf(hVideo, ubDestBuf, ulTmpLength, MMP_MEDIA_NEAR_RING_BUFFER);
                        ubVideoCurTrackTmp = m_ubVideoCurTrack;
                        m_ubVideoCurTrack = ubCurVidIndex;
                        if (MMP_ERR_NONE !=
                            m_vcodec.MMPF_VIDDEC_Init(hVideo, ubDestBuf, ulTmpLength, (MMP_ULONG)m_VideoStreamBuf, m_ulVideoStreamLen, MMP_TRUE)) {
                            return MMP_VID_UNSUPPORTED_VIDEO;
                        }
                        m_ubVideoCurTrack = ubVideoCurTrackTmp;                        
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo, &gVidFileInfo.video_info[ubCurVidIndex]);
                    }
                    
                    gVidFileInfo.video_info[ubCurVidIndex].width = 	gVidFileInfo.video_info[ubCurVidIndex].width - 
                                                            		gVidFileInfo.video_info[ubCurVidIndex].cropping_left -
                                                            		gVidFileInfo.video_info[ubCurVidIndex].cropping_right;
                                                            
                    gVidFileInfo.video_info[ubCurVidIndex].height = gVidFileInfo.video_info[ubCurVidIndex].height - 
                                                            		gVidFileInfo.video_info[ubCurVidIndex].cropping_top -
                                                            		gVidFileInfo.video_info[ubCurVidIndex].cropping_bottom;
                    
                    m_ulCurFileOffset += ulLength - 102 - 8 - ulSkipLen;  
                }
                else if (gVidFileInfo.video_info[ubCurVidIndex].format == MMP_VID_VIDEO_MJPG) 
                {
                    m_ulCurFileOffset += ulLength - 44 - 8;
                    
                    if (bInitCodec)
                    {
                        extern MMP_HANDLE          *hVideo;
                        extern MMPF_VIDEO_FUNC     m_vcodec;
                        
                        hVideo = MMPF_VIDEO_Open(CODEC_MJPG, &m_vcodec);
                        
                        if (MMP_ERR_NONE !=
                            m_vcodec.MMPF_VIDDEC_Init(hVideo,ubDestBuf, ulTmpLength,
                                                   gVidFileInfo.video_info[ubCurVidIndex].width,
                                                   gVidFileInfo.video_info[ubCurVidIndex].height,MMP_TRUE)) {//MMPF_MJPGDEC_Init
                                                  
                            DBG_S(3,"Init MJPEG failed\r\n");
                            bPlayAudioOnly = MMP_TRUE;
                        }
                        
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo, &gVidFileInfo.video_info[ubCurVidIndex]);
                    }
                }
                else {
                    m_ulCurFileOffset += ulLength - 44 - 8;
                    
                    // Set the macro blocks here so that the config won't change when playing
                    sMp4vConfig.usHMB = (gVidFileInfo.video_info[ubCurVidIndex].width + 0xF) >> 4;
                    sMp4vConfig.usVMB = (gVidFileInfo.video_info[ubCurVidIndex].height + 0xF) >> 4;

                    if (bInitCodec)
                    {
                        MMP_USHORT twidth   = sMp4vConfig.usHMB * 16;
                        MMP_USHORT theight  = sMp4vConfig.usVMB * 16;
                        
                        hVideo = MMPF_VIDEO_Open(CODEC_H263, &m_vcodec);
                        
                        if (MMP_ERR_NONE !=
                            m_vcodec.MMPF_VIDDEC_Init(hVideo, ubDestBuf, ulTmpLength, twidth, theight, MMP_TRUE)) {
                            DBG_S(3,"Init H263 failed\r\n");
                            bPlayAudioOnly = MMP_TRUE;
                        }
                        
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo, &gVidFileInfo.video_info[ubCurVidIndex]);
                    }
                }
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;   
        //"stts" Time To Sample Atom [P:stbl, C:NULL]
        case ATOM_STTS:
        	DBG_S3("3GPPSR_ParseEx ATOM_STTS\r\n");
        	
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || 
#if (MAX_VID_TK == 1)
            	parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
#if (EN_SPEED_UP_VID & CACHE_HDL)
				MMPF_VIDPSR_InitParseObjByTrack(parsingTrack, MMPF_3GP_ATOM_STTS);
#endif				
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ, 4 + 4, &ulEntryCnt);
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                if (ulEntryCnt == 1) {
                
                    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 4, 0, &MediaInfo->ulSttsDuration);
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }
                    
                    if (((MediaInfo->ulSamplePerFrame) && (MediaInfo->ulBytesPerFrame)) && (MediaInfo->ulSttsDuration == 1)) {
                        MediaInfo->ulSttsDuration *= MediaInfo->ulSamplePerFrame; //CHECK!
                    }
                        
                    m_ulFirstTime = MediaInfo->ulSttsDuration;
                }
                else {
                    error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 4, 0, &m_ulFirstTime);
                    
                    if (error != MMP_ERR_NONE) {
                        return error;
                    }    
                }
                
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STTS] = m_ulCurFileOffset;
                MediaInfo->ulSttsEntryCnt = ulEntryCnt;
                
                m_ulCurFileOffset += (8 * ulEntryCnt);
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;
        //"ctts" Composition Time To Sample Atom [P:stbl, C:NULL]    
        case ATOM_CTTS:
            DBG_S3("3GPPSR_ParseEx ATOM_CTTS\r\n");
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || 
#if (MAX_VID_TK == 1)
            	parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ, 4 + 4, &ulEntryCnt);
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                    
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + 4, 0, &m_ulFirstCTime);
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_CTTS] = m_ulCurFileOffset;
                MediaInfo->bCttsEnable = MMP_TRUE;
                MediaInfo->ulCttsEntryCnt = ulEntryCnt;
                
                m_ulCurFileOffset += (8 * ulEntryCnt);
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;
        //"stsc" Sample To Chunk Atom [P:stbl, C:NULL]
        case ATOM_STSC:
            DBG_S3("3GPPSR_ParseEx ATOM_STSC\r\n");
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || 
#if (MAX_VID_TK == 1)
            	parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
#if (EN_SPEED_UP_VID & CACHE_HDL)	                
				MMPF_VIDPSR_InitParseObjByTrack(parsingTrack, MMPF_3GP_ATOM_STSC);				
#endif				
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ, 4 + 4, &MediaInfo->ulStscEntryCnt);
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSC] = m_ulCurFileOffset;
                m_ulCurFileOffset += ulLength - 16;
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;       
        //"stsz" Sample Size Atom [P:stbl, C:NULL]
        case ATOM_STSZ:
            DBG_S3("3GPPSR_ParseEx ATOM_STSZ\r\n");
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || 
#if (MAX_VID_TK == 1)
            	parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
#if (EN_SPEED_UP_VID & CACHE_HDL)        
				MMPF_VIDPSR_InitParseObjByTrack(parsingTrack, MMPF_3GP_ATOM_STSZ);
#endif				
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ, 4 + 4, (MMP_ULONG*)&(MediaInfo->usSampleSize));
                if (error != MMP_ERR_NONE) {
                    return error;
            	}
            	
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset, 4, &(MediaInfo->ulFrameCount));
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                    
                // For MP3 Issue
                if ((MediaInfo->ulSamplePerFrame) && (MediaInfo->ulBytesPerFrame) && (MediaInfo->usSampleSize == 1)) {
					MediaInfo->usSampleSize = MediaInfo->ulBytesPerFrame;
                    MediaInfo->ulFrameCount /= MediaInfo->ulSamplePerFrame;
                }
                // For PCM/ADPCM
                else if (ulSampleSize) {
                    MediaInfo->usSampleSize = ulSampleSize;
                }
                
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSZ] = m_ulCurFileOffset;
                MediaInfo->ulStszEntryCnt = MediaInfo->ulFrameCount;
                
                // Calculate media total size
        		if (MediaInfo->usSampleSize) {
        			// Constant sample size
        			MediaInfo->ulLength = MediaInfo->usSampleSize * MediaInfo->ulFrameCount;
        		}
        		else {
        			// Variable sample size
        		   	MediaInfo->ulLength = 0;
        		   	
        			for (i = 0; i < MediaInfo->ulFrameCount; i++) {
        			
                        MMP_UBYTE ptr[4];
                    
                        error = MMPF_VIDPSR_ReadFile(ptr, MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSZ] + (4 * i), 4);
                        if (error != MMP_ERR_NONE) {
                            return error;
                        }
                        
        				MediaInfo->ulLength += MMPF_3GPPSR_Ptr2Int(ptr);
        			}
        		}
        		
                m_ulCurFileOffset += ulLength - 20;
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;         
        //"stco" Chunk Offset Atom [P:stbl, C:NULL]
        case ATOM_STCO:
            DBG_S3("3GPPSR_ParseEx ATOM_STCO\r\n");
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || 
#if (MAX_VID_TK == 1)
            	parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif            
#if (EN_SPEED_UP_VID & CACHE_HDL)
				MMPF_VIDPSR_InitParseObjByTrack(parsingTrack, MMPF_3GP_ATOM_STCO);
#endif				
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ, 4 + 4, &(MediaInfo->ulStcoEntryCnt));
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STCO] = m_ulCurFileOffset;
                m_ulCurFileOffset += ulLength - 16;
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;
        //"stss" Sync Sample Atom [P:stbl, C:NULL] contains video keyframe    
        case ATOM_STSS:
            DBG_S3("3GPPSR_ParseEx ATOM_STSS\r\n");
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || 
#if (MAX_VID_TK == 1)
            	parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 ||
            	parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif
            
                error = MMPF_3GPPSR_GetBigLong(m_ulCurFileOffset + ATOM_VER_FLAG_SZ, 4 + 4, &MediaInfo->ulStssEntryCnt);
                if (error != MMP_ERR_NONE) {
                    return error;
                }
                    
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSS] = m_ulCurFileOffset;
                m_ulCurFileOffset += ulLength - 16;
#if (MAX_VID_TK == 1)
                if (parsingTrack == MMPF_3GP_TRACK_VIDEO) {
#else                
                if (parsingTrack == MMPF_3GP_TRACK_VIDEO_TK0 || parsingTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
#endif            
                    gVidFileInfo.video_info[ubCurVidIndex].total_keyframe_num = MediaInfo->ulStssEntryCnt;
                    gVidFileInfo.is_video_seekable[ubCurVidIndex] = MMP_TRUE;
                    
                    if (gVidFileInfo.video_info[ubCurVidIndex].total_keyframe_num <= 1) {
                        gVidFileInfo.is_video_seekable[ubCurVidIndex] = MMP_FALSE;
                    }    
                }
            }
            else {
                m_ulCurFileOffset += ulLength - 8;
            }
            break;
        //"udta" User data Atom [P:moov, C:META] contains video user data
        case ATOM_UDTA:
        	DBG_S3("3GPPSR_ParseEx ATOM_UDTA\r\n");
            MMPF_3GPPSR_ParseUserData(ulLength - 8);       
            break;
        default:
        	DBG_S3("3GPPSR_ParseEx DEFAULT\r\n");
            m_ulCurFileOffset += ulLength - 8;
			//TL: There are many dummy data when SPO happend.
			//Those data size are zero so that "while loop" can't be interrupted.
			if(!ulLength) {
				m_ulCurFileOffset = ulTotalFileSize;
			}            
            break;
        }
    }

	// Check the track validation
    for (i = 0; i < m_ulVideoTrackTotalCnt; i++) {
        gVidFileInfo.video_info[i].fps = 0;

        if (bPlayAudioOnly) {
            DBG_S3("Unsupport video format!\r\n");
            gVidFileInfo.is_video_available[i] = MMP_FALSE;
            gVidFileInfo.video_info[i].format = MMP_VID_VIDEO_UNSUPPORTED;
        }
    }
    
    for (i = 0; i < m_ulVideoTrackTotalCnt; i++) {
        if (gVidFileInfo.is_video_available[i]) {
            if ((gVideoMedia[i].ulSTOffset[MMPF_3GP_ATOM_STSC] || gVideoMedia[i].ulSTOffset[MMPF_3GP_ATOM_STCO] || 
                 gVideoMedia[i].ulSTOffset[MMPF_3GP_ATOM_STSZ] || gVideoMedia[i].ulSTOffset[MMPF_3GP_ATOM_STTS]) == 0) {
                
                gVidFileInfo.is_video_available[i] = MMP_FALSE;
                m_ulVideoTrackTotalCnt--;
            }
        }
    }
	
    gVidFileInfo.videoTrackNum = m_ulVideoTrackTotalCnt;

    // Calculate video FPS, bitrate and total time
    for (i = 0; i < m_ulVideoTrackTotalCnt; i++) {
    
        if (gVidFileInfo.is_video_available[i]) {
        
            if (gVideoMedia[i].ulTimeScale == 0) {
                return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
            }
              
            gVideoMedia[i].lDelayTime = 0;
            
            gVidFileInfo.video_info[i].total_video_time /= gVideoMedia[i].ulTimeScale; //Unit:second
             
            if (gVidFileInfo.video_info[i].total_video_time != 0)
            {       
                gVidFileInfo.video_info[i].bitrate = (gVideoMedia[i].ulLength / gVidFileInfo.video_info[i].total_video_time) * 8;
                        
                gVidFileInfo.video_info[i].fps = gVideoMedia[i].ulFrameCount / gVidFileInfo.video_info[i].total_video_time;
                
                gVidFileInfo.video_info[i].total_video_time *= 1000; //Unit:ms
            }
            else {
                gVidFileInfo.video_info[i].bitrate = 0;
                gVidFileInfo.video_info[i].fps     = 0;
            }
            
            gVideoMedia[i].lMediaTime = (gVideoMedia[i].lMediaTime * 1000) / gVideoMedia[i].ulTimeScale;
            
            if (gVidFileInfo.is_audio_available && gAudioMedia.ulTimeScale != 0) {
                gAudioMedia.lMediaTime = (gAudioMedia.lMediaTime * 1000) / gAudioMedia.ulTimeScale;
            }
            
            gVideoMedia[i].lDelayTime = gVideoMedia[i].lMediaTime - gAudioMedia.lMediaTime;
        }  
    }

    if (gVidFileInfo.is_video_available[0] == MMP_FALSE && 
        gVidFileInfo.is_video_available[1] == MMP_FALSE && 
    	gVidFileInfo.is_audio_available == MMP_FALSE) {
        DBG_S3("NO AV track!\r\n");
        return MMP_VID_NO_AV_TRACK;
    }

    // Calculate audio sampling rate, bitrate and total time
    gVidFileInfo.audio_info.sampling_rate = 0;
    
    if (!bSupportAudio) {
        gVidFileInfo.audio_info.format = MMP_VID_AUDIO_NONE;
        gVidFileInfo.is_audio_available = MMP_FALSE;
    }
    
    if (gVidFileInfo.is_audio_available) {
    
        gAudioMedia.lDelayTime = 0;
    
        if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AAC_LC) {
            if (mp4ASC.samplingFrequency == 0) {
                mp4ASC.samplingFrequency = gAudioMedia.ulTimeScale;
            }
            gVidFileInfo.audio_info.sampling_rate = mp4ASC.samplingFrequency;
        }
        #if (VAMR_P_EN)
        else if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AMR_NB) {
            gVidFileInfo.audio_info.sampling_rate = 8000;
        }
        #endif
        #if (VMP3_P_EN)
        else if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_MP3) {
            MMPF_SetMP3Layer(0x03);
        }
        #endif
        else {
            gVidFileInfo.audio_info.sampling_rate = gAudioMedia.ulTimeScale;
        }
        
        if ((gAudioMedia.ulSTOffset[MMPF_3GP_ATOM_STSC] && gAudioMedia.ulSTOffset[MMPF_3GP_ATOM_STCO] && 
             gAudioMedia.ulSTOffset[MMPF_3GP_ATOM_STSZ]) == 0) {
            return MMP_VID_UNSUPPORTED_AUDIO;
        }
        //TL: If total_audio_time = 0 was not supported, thumbnail will fail.
        if (!gAudioMedia.ulTimeScale) {
			if(!gVidFileInfo.audio_info.total_audio_time) {
				printc(FG_YELLOW("[Warning] total_audio_time = 0\r\n"));
			}
            return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
        }
                   
        gVidFileInfo.audio_info.total_audio_time /= gAudioMedia.ulTimeScale;
    
        if (gVidFileInfo.audio_info.total_audio_time != 0)
        {
            gVidFileInfo.audio_info.bitrate = (gAudioMedia.ulLength / gVidFileInfo.audio_info.total_audio_time) * 8;
            gVidFileInfo.audio_info.total_audio_time *= 1000;
        }
        else {
            gVidFileInfo.audio_info.bitrate = 0;
        }
        
        m_ulAudSampleRate = gVidFileInfo.audio_info.sampling_rate;
        
        #if (VWAV_P_EN)
        if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_WAV)
        {
            WAVFileInfo 	*wavinfo;
            AIT_AdpcmDecCtx *tContext;
            
            MMPF_GetWAVInfo(&wavinfo);

            wavinfo->bitRate 			= gVidFileInfo.audio_info.bitrate;
            wavinfo->totaldataSize 		= 0;
            wavinfo->compressionCode 	= 0;
            wavinfo->nChannels 			= gVidFileInfo.audio_info.channel_nums;
            wavinfo->sampleRate 		= gVidFileInfo.audio_info.sampling_rate;
            wavinfo->newSampleRate 		= gVidFileInfo.audio_info.sampling_rate;
            wavinfo->bitsPerSample 		= gVidFileInfo.audio_info.bits_per_coded_sample;
            
            if (usWavId != 0) {
                MMPF_SetWAVFormat(MMP_TRUE);
                MMPF_GetWAVContext(&tContext);
                
                tContext->AdpcmDCtxnCh = gVidFileInfo.audio_info.channel_nums;
                
                if (usWavId == 6 || usWavId == 7) {
                    tContext->AdpcmDCtxBlkAlign = 2;
                } 
                else {
                    tContext->AdpcmDCtxBlkAlign = gVidFileInfo.audio_info.block_align;
                }
                
                if (usWavId == 0x12)
                    tContext->AdpcmDCtxCID = 0x11000;
                else
                    tContext->AdpcmDCtxCID = usWavId;
                
                tContext->AdpcmDCtxBitPerEncSample 	= gVidFileInfo.audio_info.bits_per_coded_sample;
                tContext->AdpcmDCtxSampleRate 		= gVidFileInfo.audio_info.sampling_rate;
                tContext->AdpcmDCtxBitRate 			= gVidFileInfo.audio_info.bitrate;
                DBG_S(3, "Set ADPCM Context Done\r\n");
            }
        }
        #endif
    }
    
    if (pInfo) {
        MEMCPY(pInfo, &gVidFileInfo, sizeof(MMP_VIDEO_CONTAINER_INFO));
    }
    
    return fileValidation;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_Parse
//  Description :
//------------------------------------------------------------------------------
MMP_VID_FILEVALIDATION MMPF_3GPPSR_Parse(void)
{
	return MMPF_3GPPSR_ParseEx(NULL, MMP_TRUE);
}

//------------------------------------------------------------------------------
//  Function    : MMPF_3GPPSR_InitVideoCodec
//  Description :
//------------------------------------------------------------------------------
/** @brief Initial video after select the video track.
@return 
*/
MMP_ERR MMPF_3GPPSR_InitVideoCodec(MMP_UBYTE ubIndex)
{    
    MMP_UBYTE  *ubDestBuf = m_VideoStreamBuf;

    if (m_ulCodecType == MMP_VID_VIDEO_MJPG) {
        //MMPF_SYS_EnableClock(MMPF_SYS_CLK_JPG, MMP_TRUE);
        
        //hVideo = MMPF_VIDEO_Open(CODEC_MJPG, &m_vcodec);
        
        //MEMCPY((MMP_UBYTE *)ubDestBuf, &m_VideoInfo[ubIndex][0], m_VideoInfoLength[ubIndex]);
        
        //m_vcodec.MMPF_VIDDEC_ConnectBSBuf(hVideo, ubDestBuf, m_VideoInfoLength[ubIndex], NULL);//MMPF_MJPGDEC_ConnectBSBuf
        
        //if (MMP_ERR_NONE !=
        //    m_vcodec.MMPF_VIDDEC_Init(hVideo,ubDestBuf, m_VideoInfoLength[ubIndex], (MMP_ULONG)m_VideoStreamBuf, m_ulVideoStreamLen,MMP_TRUE)) {//MMPF_MJPGDEC_Init
        //    MMPF_SYS_EnableClock(MMPF_SYS_CLK_JPG, MMP_FALSE);
        //    return MMP_VID_UNSUPPORTED_VIDEO;
        //}
        
        //MMPF_SYS_EnableClock(MMPF_SYS_CLK_JPG, MMP_FALSE);
    }
    else {
        MMPF_SYS_EnableClock(MMPF_SYS_CLK_H264, MMP_TRUE);
        
        hVideo = MMPF_VIDEO_Open(CODEC_H264, &m_vcodec);
        
        MEMCPY((MMP_UBYTE *)ubDestBuf, &m_VideoInfo[ubIndex][0], m_VideoInfoLength[ubIndex]);
        
        m_vcodec.MMPF_VIDDEC_ConnectBSBuf(hVideo, ubDestBuf, m_VideoInfoLength[ubIndex], MMP_MEDIA_NEAR_RING_BUFFER);//MMPF_H264DEC_ConnectBSBuf
        
        if (MMP_ERR_NONE !=
            m_vcodec.MMPF_VIDDEC_Init(hVideo, ubDestBuf, m_VideoInfoLength[ubIndex], (MMP_ULONG)m_VideoStreamBuf, m_ulVideoStreamLen, MMP_TRUE)) {//MMPF_H264DEC_Init
            MMPF_SYS_EnableClock(MMPF_SYS_CLK_H264, MMP_FALSE);
            return MMP_VID_UNSUPPORTED_VIDEO;
        }
        
        MMPF_SYS_EnableClock(MMPF_SYS_CLK_H264, MMP_FALSE);
    }
    return MMP_ERR_NONE;
}

#if 0
void _____ParserAPIs_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : MMPF_PSR_SetVideoInBuffers
//  Description :
//------------------------------------------------------------------------------
/** @brief Set the video input buffer addresses

The 2 parameters are given by the host.
It should be any place which is not used before fopen and play.
@param[in] ulVideoInAddr The starting address of the video in (compressed) buffer.
@param[in] ulLength The length of the file name.
*/
void MMPF_PSR_SetVideoInBuffers(const MMP_ULONG ulVideoInAddr, const MMP_ULONG ulLength, MMP_ULONG buffer_type)
{
    m_bVideoEOS         = MMP_FALSE;
    m_bAudioEOS         = MMP_FALSE;
    
    m_ulBsBufType       = buffer_type;
    m_VideoStreamBuf    = (MMP_UBYTE *) ulVideoInAddr;
    m_ulVideoStreamLen	= ulLength;
    
    if (m_ulBsBufType == MMP_MEDIA_NEAR_RING_BUFFER) {
        m_bBsSupportLargeFrame = MMP_FALSE;
    }
    else if (m_ulBsBufType == MMP_MEDIA_RING_BUFFER) {
        m_bBsSupportLargeFrame = MMP_TRUE;
    }
}

//------------------------------------------------------------------------------
//  Function    : MMPF_PSR_Reset
//  Description :
//------------------------------------------------------------------------------
void MMPF_PSR_Reset(void)
{
    MEMSET0(&m_3GPPdata);
    
    m_bVideoEOS = MMP_FALSE;
    m_bAudioEOS = MMP_FALSE;
}
#if (EN_SPEED_UP_VID & CACHE_HDL)
//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_FileObjCopy
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_VIDPSR_FileObjCopy(const MMP_ULONG ulIndexTo, const MMP_ULONG ulIndexFrom)
{
   	MEMCPY((&m_VideoFile[ulIndexTo]), m_sDepackCache[ulIndexFrom].fileHandle, sizeof(MMP_FS_FILE));
    MEMCPY((&m_VideoFileObj[ulIndexTo]), ((MMP_FS_FILE*)m_sDepackCache[ulIndexFrom].fileHandle)->pFileObj, sizeof(MMP_FS_FILE_OBJ));        
    m_VideoFile[ulIndexTo].pFileObj = &m_VideoFileObj[ulIndexTo];
  	m_sDepackCache[ulIndexTo].fileHandle = &(m_VideoFile[ulIndexTo]);    
    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPF_VIDPSR_InitParseObjByTrack
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPF_VIDPSR_InitParseObjByTrack(MMP_ULONG ulTrack, MMP_ULONG ulCurAtomType)
{
	MMP_ULONG ulCacheType;
	
    m_ul3gpAtomParseOrder[ulTrack][m_ubAtomOrderByTrk[ulTrack]].ulCurAtom = ulCurAtomType;
    m_ubAtomOrderByTrk[ulTrack]++;	
	//if (m_ubAtomOrderByTrk[ulTrack] == 1) 
    {
    	if (ulTrack == MMPF_3GP_TRACK_AUDIO) {
        	 //ulCurAtomType = m_ul3gpAtomParseOrder[ulTrack][m_ubAtomOrderByTrk[ulTrack]].ulCurAtom;
             ulCacheType = AUDIO_CHUNK_CACHE + (ulCurAtomType * MAX_TRACK_NUM);                     
             MMPF_VIDPSR_FileObjCopy(ulCacheType, m_ulCacheIdx);
        }
        else if (ulTrack == MMPF_3GP_TRACK_VIDEO_TK0) {
             //m_ul3gpAtomParseOrder[ulTrack][m_ubAtomOrderByTrk[ulTrack]].ulCurAtom;
             ulCacheType = VIDEO_CHUNK_CACHE_TK0 + (ulCurAtomType * MAX_TRACK_NUM); 
             MMPF_VIDPSR_FileObjCopy(ulCacheType, m_ulCacheIdx);                
        }
        else if (ulTrack == MMPF_3GP_TRACK_VIDEO_TK1) {
             //m_ul3gpAtomParseOrder[ulTrack][m_ubAtomOrderByTrk[ulTrack]].ulCurAtom;
             ulCacheType = VIDEO_CHUNK_CACHE_TK1 + (ulCurAtomType * MAX_TRACK_NUM); 
             MMPF_VIDPSR_FileObjCopy(ulCacheType, m_ulCacheIdx);                 
        }
    }
}	
#endif //#if (EN_SPEED_UP_VID & CACHE_HDL)
#endif
#endif
